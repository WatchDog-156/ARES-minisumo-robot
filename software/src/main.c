#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/uart.h"
#include "inc/motors.h"
#include "inc/serwo.h"
#include "lineDetectors.h"


#define UART_ID uart1
#define BAUD_RATE 115200


int main()
{
    stdio_init_all();
    motor_init();
    serwo_init();
  // Konfiguracja LED
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    printf("System started. LED is ON.\n");
   
    while (true) {
        sleep_ms(3000);
        serwo_set_posiotion(LEFT_SERWO, 175);
        serwo_set_posiotion(RIGHT_SERWO, 5);
        sleep_ms(500);
        motor_set(Left_Motor, Forward, 90);
        motor_set(Right_Motor, Forward, 90);
        sleep_ms(3000);
        motor_stop(Left_Motor);
        motor_stop(Right_Motor);
        sleep_ms(500);
        motor_set(Left_Motor, Backward, 90);
        motor_set(Right_Motor, Backward, 90);
        sleep_ms(3000);
        motor_stop(Left_Motor);
        motor_stop(Right_Motor);
        sleep_ms(500);
        serwo_set_posiotion(LEFT_SERWO, 90);
        serwo_set_posiotion(RIGHT_SERWO, 90);
    }
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
