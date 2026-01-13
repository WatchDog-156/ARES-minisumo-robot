#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "btstack.h"
#include "line_detectors.h"
#include "bluetooth_connection.h"
#include "motors.h"
#include "serwo.h"
#include "distance_sensor.h"

static btstack_timer_source_t sensor_timer;
static void sensor_timer_handler(btstack_timer_source_t *ts) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, !cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN));
    int dystans[4];
    uint8_t addr[4]={0x30, 0x31, 0x32, 0x33};

    // Odczyt czujników
    int status_left = read_detector_value(Left);
    int status_right = read_detector_value(Right);
    for (int i=0; i<4; i++){
        tofSetCurrentAddress(addr[i]);
        dystans[i]=tofReadDistance(i);
    }

    // Wysłanie telemetrii przez Bluetooth
    char msg[64];
    snprintf(msg, sizeof(msg), "L:%d R:%d\n", status_left, status_right);
    printf("%s", msg);
    char msg2[128];

    snprintf(msg2, sizeof(msg2), "XShut1: %d XShut2: %d XShut3: %d XShut4: %d\n", dystans[0], dystans[1], dystans[2], dystans[3]);
    printf("%s", msg2);
    bluetooth_send_telemetry(msg);
    bluetooth_send_telemetry(msg2);

    // Resetowanie timera na 500 ms
    btstack_run_loop_set_timer(ts, 500);
    btstack_run_loop_add_timer(ts);
}


#define UART_ID uart1
#define BAUD_RATE 115200


int main()
{
    stdio_init_all();

    // Krótsze opóźnienie na start USB

    sleep_ms(3000);
    printf("=== MiniSumo Robot Starting ===\n");

    // 1. Inicjalizacja CYW43 (WiFi/BT chip)
    if (cyw43_arch_init()) {
    printf("ERROR: cyw43_arch_init failed!\n");
    return -1;
    }

    printf("CYW43 initialized OK\n");



    // 2. Włączenie LED
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    printf("LED is ON\n");

    // 3. Inicjalizacja czujników linii
    line_detectors_init();
    printf("Line detectors initialized\n");
    motor_init();
    serwo_init();
    IR_init();




    // 4. Inicjalizacja Bluetooth
    bluetooth_init();
    printf("Bluetooth initialized and advertising...\n");



    // 5. Konfiguracja timera dla czujników
    sensor_timer.process = &sensor_timer_handler;
    btstack_run_loop_set_timer(&sensor_timer, 500);
    btstack_run_loop_add_timer(&sensor_timer);
    printf("Sensor timer configured\n");

    // 6. Start głównej pętli BTstack
    printf("Starting BTstack main loop...\n");
    printf("============================\n\n");
    btstack_run_loop_execute();

    // Ten kod nigdy się nie wykona
    return 0;
} 