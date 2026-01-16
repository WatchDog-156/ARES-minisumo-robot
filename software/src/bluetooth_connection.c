#include "bluetooth_connection.h"
#include <stdio.h>
#include <string.h> 
#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "minisumo.h"
#include "motors.h"
#include "serwo.h"
#include "starter.h"

static hci_con_handle_t connection_handle = HCI_CON_HANDLE_INVALID;
static btstack_packet_callback_registration_t hci_event_callback_registration;

// callback, gdy klient chce odczytać dane (Pico->Telefon)
static uint16_t att_read_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t offset, uint8_t *buffer, uint16_t buffer_size) {
    return 0;
}

void parse_long_command(const char* input_cmd){
    char *words[10];
    int word_count = 0;
    char cmd[64];

    strncpy(cmd, input_cmd, sizeof(cmd) - 1);
    cmd[sizeof(cmd) - 1] = 0;
    cmd[strcspn(cmd, "\r\n")] = 0;
    char *word = strtok(cmd, " ");
    while (word != NULL && word_count < 10) {
        words[word_count++] = word;
        word = strtok(NULL, " ");
    }

    for(int word_idx = 0; word_idx<word_count; word_idx++){
        char msg[64];
        if(strncmp(words[word_idx], "LEFT", 4) == 0){
            if(word_idx+1<word_count){
                char *endptr;
                int speed = strtol(words[word_idx + 1], &endptr, 10);
                if(endptr == words[word_idx + 1]){
                    printf("-> Nieznana komenda\n");
                    snprintf(msg, sizeof(msg), "Niepoprawna wartość dla komendy: %s\n", words[word_idx]);
                    bluetooth_send_telemetry(msg);
                }else {
                    if(speed<0)
                        motor_set(Left_Motor, Backward, abs(speed));
                    else if(speed>0)
                        motor_set(Left_Motor, Forward, speed);
                    else 
                        motor_stop(Left_Motor);
                    printf("-> Lewy silnik ustalono na %s\n", words[word_idx+1]);
                    snprintf(msg, sizeof(msg), "Odebrano: %s %s\n", words[word_idx], words[word_idx+1]);
                    bluetooth_send_telemetry(msg);
                    word_idx++;
                }
            }else{
                printf("-> Nieznana komenda\n");
                snprintf(msg, sizeof(msg), "Brak wartości dla komendy: %s\n", words[word_idx]);
                bluetooth_send_telemetry(msg);
            }
        }else if(strncmp(words[word_idx], "RIGHT", 5) == 0){
            if(word_idx+1<word_count){
                char *endptr;
                int speed = strtol(words[word_idx + 1], &endptr, 10);
                if(endptr == words[word_idx + 1]){
                    printf("-> Nieznana komenda\n");
                    snprintf(msg, sizeof(msg), "Niepoprawna wartość dla komendy: %s\n", words[word_idx]);
                    bluetooth_send_telemetry(msg);
                }else {
                    if(speed<0)
                        motor_set(Right_Motor, Backward, abs(speed));
                    else if(speed>0)
                        motor_set(Right_Motor, Forward, speed);
                    else 
                        motor_stop(Right_Motor);
                    printf("-> Prawy silnik ustalono na %s\n", words[word_idx+1]);
                    snprintf(msg, sizeof(msg), "Odebrano: %s %s\n", words[word_idx], words[word_idx+1]);
                    bluetooth_send_telemetry(msg);
                    word_idx++;
                }
            }else{
                printf("-> Nieznana komenda\n");
                snprintf(msg, sizeof(msg), "Brak wartości dla komendy: %s\n", words[word_idx]);
                bluetooth_send_telemetry(msg);
            }
        }else{
            printf("-> Nieznana komenda\n");
            snprintf(msg, sizeof(msg), "Nieznana komenda: %s\n", words[word_idx]);
            bluetooth_send_telemetry(msg);
        }
    }
}

// callback, gdy klient wysyła dane (Telefon->Pico) 
static int att_write_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size) {
    // charakterystyka RX
    if (att_handle == ATT_CHARACTERISTIC_6E400002_B5A3_F393_E0A9_E50E24DCCA9E_01_VALUE_HANDLE) {
        char cmd[64];
        
        uint16_t len;
        if(buffer_size<63)
            len = buffer_size;
        else 
            len = 63;
        
        memcpy(cmd, buffer, len);
        cmd[len] = '\0';

        printf("BT RX: '%s' (%d bytes)\n", cmd, buffer_size);

        if (strncmp(cmd, "START", 5) == 0) {
            printf("-> Start pracy robota!\n");
            changeState(Fighting);
        } 
        else if (strncmp(cmd, "END", 3) == 0) {
            printf("-> Koniec pracy robota!\n");
            changeState(End);
        }
        else if (strncmp(cmd, "STOP", 4) == 0) {
            printf("-> Zatrzymano silniki!\n");
            all_motor_stop();
        }
        else if (strncmp(cmd, "FWD", 3) == 0) {
            printf("-> Jazda do przodu!\n");
            motor_set(Left_Motor, Forward, 50);
            motor_set(Right_Motor, Forward, 50);
        }
        else if (strncmp(cmd, "BWD", 3) == 0) {
            printf("-> Jazda do tyłu!\n");
            motor_set(Left_Motor, Backward, 50);
            motor_set(Right_Motor, Backward, 50);
        }
        else if (strncmp(cmd, "SER_UP", 6) == 0) {
            printf("-> Serwa opuszczone!\n");
            serwo_set_posiotion(LEFT_SERWO, 90);
            serwo_set_posiotion(RIGHT_SERWO, 90);
        }
        else if (strncmp(cmd, "SER_DW", 6) == 0) {
            printf("-> Serwa podniesione!\n");
            serwo_set_posiotion(LEFT_SERWO, 170);
            serwo_set_posiotion(RIGHT_SERWO, 10);
        }
        else {
            parse_long_command(cmd);
            return 0;
        }

        char msg[64];
        snprintf(msg, sizeof(msg), "Odebrano: %s", cmd);
        bluetooth_send_telemetry(msg);
    }
    return 0;
}

// obsługa zdarzeń
static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(channel);
    UNUSED(size);
    
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (hci_event_packet_get_type(packet)) {
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
                printf("BT: Stack is working\n");
            }
            break;

        case HCI_EVENT_LE_META:
            if(hci_event_le_meta_get_subevent_code(packet)==HCI_SUBEVENT_LE_CONNECTION_COMPLETE) {
                    connection_handle = hci_subevent_le_connection_complete_get_connection_handle(packet);
                    printf("BT: Device connected\n");
                    gap_advertisements_enable(0);
            }
            break;

        case HCI_EVENT_DISCONNECTION_COMPLETE:
            printf("BT: Device disconnected\n");
            connection_handle = HCI_CON_HANDLE_INVALID;
            gap_advertisements_enable(1);
            break;

        default:
            break;
    }
}

// konfiguracja reklamy (Advertising)
static void setup_advertising(void){
    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0030;
    uint8_t adv_type = 0;
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);

    // Advertising data
    uint8_t adv_data[] = {
        // Flagi
        0x02, 0x01, 0x06,
        // Nazwa urządzenia
        0x09, 0x09, 'M', 'i', 'n', 'i', 'S', 'u', 'm', 'o',
        // Serwis UUID - Nordic UART Service (zapisany od końca)
        0x11, 0x06,
        0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 
        0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 
        0x01, 0x00, 0x40, 0x6E
    };

    gap_advertisements_set_data(sizeof(adv_data), adv_data);
    printf("BT: Advertising data\n");
}

// init Bluetooth
void bluetooth_init(void){
    l2cap_init();
    sm_init();

    printf("BT: Initializing ATT Server\n");
    att_server_init(profile_data, att_read_callback, att_write_callback);

    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    setup_advertising();
    gap_advertisements_enable(1);

    hci_power_control(HCI_POWER_ON);
    
    printf("BT: Initialization complete!\n");
}

// wysyłanie danych po Bluetooth
int bluetooth_send_telemetry(const char *data){
    if (connection_handle == HCI_CON_HANDLE_INVALID) {
        printf("BT: No connection, cannot send message\n");
        return 1;
    }

    // charakterystyka TX
    uint16_t value_handle = ATT_CHARACTERISTIC_6E400003_B5A3_F393_E0A9_E50E24DCCA9E_01_VALUE_HANDLE;
    
    int result = att_server_notify(connection_handle, value_handle, (uint8_t*)data, strlen(data));
    
    if (result != 0) {
        printf("BT: TX failed: %d\n", result);
    }
    
    return result;
}
