#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "btstack.h"
#include "line_detectors.h"
#include "bluetooth_connection.h"
#include "motors.h"
#include "serwo.h"

static btstack_timer_source_t sensor_timer;

static void sensor_timer_handler(btstack_timer_source_t *ts) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, !cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN));
    
    // Odczyt czujników
    int status_left = read_detector_value(Left); 
    int status_right = read_detector_value(Right); 
    
    // Wysłanie telemetrii przez Bluetooth
    char msg[64];
    snprintf(msg, sizeof(msg), "L:%d R:%d\n", status_left, status_right);
    printf("%s", msg);
    bluetooth_send_telemetry(msg);

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


// int main() {
//     stdio_init_all();
    
//     // Konfiguracja LED
//     cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
//     printf("System started. LED is ON.\n");

//     /* linedetectors test*/
//     /*line_detectors_init();
//     while (true) {
//         check_line_detectors();
//         sleep_ms(500);
//     }*/
    
//     /* IR sensors test*/
//     /*if (!sensors_IR_init()) {
//         printf("Błąd inicjalizacji czujników IR!\n");
//         return -1;
//     }
//     while (1){
//         for (uint8_t i = 0; i < 4; i++) {
//             int16_t distance = read_IR_distance_mm(i);
//             if (distance >= 0) {
//                 printf("Czujnik %d: Odległość = %d mm\n", i, distance);
//             } else {
//                 printf("Czujnik %d: Błąd odczytu!\n", i);
//             }
//         }
//         sleep_ms(1000);
//     }*/
    


//     return 0;
// }