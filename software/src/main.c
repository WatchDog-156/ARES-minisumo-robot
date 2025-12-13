#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/uart.h"
#include "inc/motors.h"
#include "inc/serwo.h"


#define UART_ID uart1
#define BAUD_RATE 115200


int main()
{
    stdio_init_all();
    motor_init();
    serwo_init();
   
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
}
