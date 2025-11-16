#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lineDetectors.h"


int main() {
    stdio_init_all();
    
    // Konfiguracja LED
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    printf("System started. LED is ON.\n");

    /* linedetectors test*/
    /*line_detectors_init();
    while (true) {
        check_line_detectors();
        sleep_ms(500);
    }*/
    
    /* IR sensors test*/
    /*if (!sensors_IR_init()) {
        printf("Błąd inicjalizacji czujników IR!\n");
        return -1;
    }
    while (1){
        for (uint8_t i = 0; i < 4; i++) {
            int16_t distance = read_IR_distance_mm(i);
            if (distance >= 0) {
                printf("Czujnik %d: Odległość = %d mm\n", i, distance);
            } else {
                printf("Czujnik %d: Błąd odczytu!\n", i);
            }
        }
        sleep_ms(1000);
    }*/
    


    return 0;
}