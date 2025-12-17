#include "bluetooth_connection.h"
#include <stdio.h>
#include <string.h> 
#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

// Dołączamy wygenerowany plik z definicjami (uchwyty ATT)
#include "minisumo.h"

static hci_con_handle_t connection_handle = HCI_CON_HANDLE_INVALID;
static btstack_packet_callback_registration_t hci_event_callback_registration;

// -------------------------------------------------------------------------
// Obsługa ODBIERANIA danych (Telefon -> Robot)
// -------------------------------------------------------------------------
static uint16_t att_read_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size) {
    // Możesz tutaj obsłużyć odczyt charakterystyk jeśli potrzebne
    return 0;
}

static int att_write_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size) {
    
    // Sprawdzamy czy zapis dotyczy charakterystyki RX
    if (att_handle == ATT_CHARACTERISTIC_6E400002_B5A3_F393_E0A9_E50E24DCCA9E_01_VALUE_HANDLE) {
        
        // Zabezpieczenie końca łańcucha
        char cmd[64];
        uint16_t len = (buffer_size < 63) ? buffer_size : 63;
        memcpy(cmd, buffer, len);
        cmd[len] = '\0';

        printf("BT RX: '%s' (%d bytes)\n", cmd, buffer_size);

        // --- LOGIKA STEROWANIA ---
        if (strncmp(cmd, "FWD", 3) == 0) {
            printf("-> Jadę do przodu!\n");
            // Tutaj wywołaj motor_forward(50); 
        } 
        else if (strncmp(cmd, "BWD", 3) == 0) {
            printf("-> Jadę do tyłu!\n");
            // motor_backward(50);
        }
        else if (strncmp(cmd, "LEFT", 4) == 0) {
            printf("-> Skręt w lewo!\n");
            // motor_turn_left(50);
        }
        else if (strncmp(cmd, "RIGHT", 5) == 0) {
            printf("-> Skręt w prawo!\n");
            // motor_turn_right(50);
        }
        else if (strncmp(cmd, "STOP", 4) == 0) {
            printf("-> Stop!\n");
            // motor_stop();
        }
        else {
            printf("-> Nieznana komenda\n");
        }
    }
    return 0;
}

// -------------------------------------------------------------------------
// Obsługa ZDARZEŃ (Połączenie, Rozłączenie)
// -------------------------------------------------------------------------
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
            switch (hci_event_le_meta_get_subevent_code(packet)) {
                case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
                    connection_handle = hci_subevent_le_connection_complete_get_connection_handle(packet);
                    printf("BT: *** POŁĄCZONO! Handle: 0x%04X ***\n", connection_handle);
                    // Zatrzymaj advertising po połączeniu
                    gap_advertisements_enable(0);
                    break;
            }
            break;

        case HCI_EVENT_DISCONNECTION_COMPLETE:
            printf("BT: *** ROZŁĄCZONO ***\n");
            connection_handle = HCI_CON_HANDLE_INVALID;
            // Wznów advertising
            gap_advertisements_enable(1);
            printf("BT: Advertising wznowione\n");
            break;

        case ATT_EVENT_CAN_SEND_NOW:
            // Można wysłać kolejne dane
            break;
    }
}

// -------------------------------------------------------------------------
// Konfiguracja Reklam (Advertising)
// -------------------------------------------------------------------------
static void setup_advertising(void){
    uint16_t adv_int_min = 0x0030; // 30ms
    uint16_t adv_int_max = 0x0030;
    uint8_t adv_type = 0; // ADV_IND
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);

    // Advertising data
    // Format: [długość, typ, dane...]
    uint8_t adv_data[] = {
        // Flags: General Discoverable + BR/EDR Not Supported
        0x02, 0x01, 0x06,
        // Complete Local Name "MiniSumo"
        0x09, 0x09, 'M', 'i', 'n', 'i', 'S', 'u', 'm', 'o',
        // Complete list of 128-bit Service UUIDs (Nordic UART Service)
        // UUID: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E (little-endian)
        0x11, 0x07,
        0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 
        0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 
        0x01, 0x00, 0x40, 0x6E
    };

    gap_advertisements_set_data(sizeof(adv_data), adv_data);
    printf("BT: Advertising data set (%d bytes)\n", sizeof(adv_data));
}

// -------------------------------------------------------------------------
// Funkcje Publiczne
// -------------------------------------------------------------------------

void bluetooth_init(void){
    printf("BT: Initializing L2CAP...\n");
    l2cap_init();
    
    printf("BT: Initializing SM...\n");
    sm_init();

    printf("BT: Initializing ATT Server...\n");
    // Inicjalizacja bazy danych GATT z wygenerowanego profilu
    att_server_init(profile_data, att_read_callback, att_write_callback);

    printf("BT: Registering HCI event handler...\n");
    // Rejestracja obsługi zdarzeń
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    printf("BT: Setting up advertising...\n");
    // Konfiguracja advertising
    setup_advertising();
    
    printf("BT: Enabling advertising...\n");
    gap_advertisements_enable(1);

    printf("BT: Powering on HCI...\n");
    // Włączenie zasilania - musi być NA KOŃCU!
    hci_power_control(HCI_POWER_ON);
    
    printf("BT: Initialization complete!\n");
}

int bluetooth_send_telemetry(const char *data){
    if (connection_handle == HCI_CON_HANDLE_INVALID) {
        // Nie połączono - nie wysyłamy
        return 1;
    }

    // Pobranie uchwytu dla charakterystyki TX
    uint16_t value_handle = ATT_CHARACTERISTIC_6E400003_B5A3_F393_E0A9_E50E24DCCA9E_01_VALUE_HANDLE;
    
    // Wysłanie notyfikacji
    int result = att_server_notify(connection_handle, value_handle, (uint8_t*)data, strlen(data));
    
    if (result != 0) {
        printf("BT TX failed: %d\n", result);
    }
    
    return result;
}
