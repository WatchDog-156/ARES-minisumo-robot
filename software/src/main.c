#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "btstack.h"
#include "bluetooth_connection.h"
#include "line_detectors.h"
#include "motors.h"
#include "serwo.h"
#include "starter.h"
#include "distance_sensor.h"
#include "controller.h"

static btstack_timer_source_t mess_timer;
static btstack_timer_source_t prog_timer;

static void mess_timer_handler(btstack_timer_source_t *ts) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, !cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN));

    // Odczyt czujników 
    int status_left = read_detector_value(Left);
    int status_right = read_detector_value(Right);

    int dystans[4];
    uint8_t addr[4]={0x30, 0x31, 0x32, 0x33};
    for (int i=0; i<4; i++){
        tofSetCurrentAddress(addr[i]);
        dystans[i]=tofReadDistance(i);
    }

    // Wysłanie telemetrii przez Bluetooth
    char msg[64]; 
    switch(getState()){
        case Start: 
            snprintf(msg, sizeof(msg), "Robot status: Start\n")
            break;
        case Fighting: 
            snprintf(msg, sizeof(msg), "Robot status: Fighting\n")
            break;
        case End: 
            snprintf(msg, sizeof(msg), "Robot status: End\n")
            break;
    }
    printf("%s", msg);

    char msg1[64];
    snprintf(msg1, sizeof(msg1), "L:%d R:%d\n", status_left, status_right);
    printf("%s", msg1);

    char msg2[128];
    snprintf(msg2, sizeof(msg2), "XShut1: %d XShut2: %d XShut3: %d XShut4: %d\n", dystans[0], dystans[1], dystans[2], dystans[3]);
    printf("%s", msg2);

    bluetooth_send_telemetry(msg);
    bluetooth_send_telemetry(msg1);
    bluetooth_send_telemetry(msg2);

    // Resetowanie timera
    btstack_run_loop_set_timer(ts, 500);
    btstack_run_loop_add_timer(ts);
}

static void prog_timer_handler(btstack_timer_source_t *ts) {
    exec_program();

    // Resetowanie timera
    btstack_run_loop_set_timer(ts, 1);
    btstack_run_loop_add_timer(ts);
}

#define UART_ID uart1
#define BAUD_RATE 115200

int main()
{
    stdio_init_all();
    sleep_ms(200);
    printf("Ares IS Starting\n");

    if (cyw43_arch_init()) {
        printf("ERROR: cyw43_arch_init failed!\n");
        return 1;
    }

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    printf("LED is on\n");

    // Inits
    line_detectors_init();
    motor_init();
    serwo_init();
    IR_init(); 
    starter_init();
    bluetooth_init();

    // Konfiguracja timera dla wiadomości Bluetooth
    mess_timer.process = &mess_timer_handler;
    btstack_run_loop_set_timer(&mess_timer, 500);
    btstack_run_loop_add_timer(&mess_timer);
    printf("Message timer configured\n");

    // Konfiguracja timera dla pracy robota
    prog_timer.process = &prog_timer_handler;
    btstack_run_loop_set_timer(&prog_timer, 1);
    btstack_run_loop_add_timer(&prog_timer);
    printf("Program timer configured\n");

    // Start głównej pętli BTstack
    printf("Starting BTstack main loop\n");
    btstack_run_loop_execute(); 
    return 0;
} 