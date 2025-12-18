#ifndef MOTORS_H
#define MOTORS_H

#include "pico/stdlib.h"
#include <stdint.h>

#define Left_Motor_PWM_Pin 10
#define Left_Motor_Dir1_Pin 12
#define Left_Motor_Dir2_Pin 11

#define Right_Motor_PWM_Pin 9
#define Right_Motor_Dir1_Pin 7
#define Right_Motor_Dir2_Pin 8

typedef enum{
    Forward,
    Backward,
    Stop,
} direction_t;

typedef enum{
    Left_Motor,
    Right_Motor
} motor_id_t;

void motor_init(void);
void motor_set(motor_id_t motor, direction_t direction, uint8_t speed);
void motor_stop(motor_id_t motor);
void all_motor_stop(void);





#endif