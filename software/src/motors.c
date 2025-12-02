#include "motors.h"
#include "hardware/pwm.h"

#define PWM_MAX_VALUE 255

typedef struct 
{
    uint pwm_pin;
    uint dir1_pin;
    uint dir2_pin;
    uint slice_num;
    uint channel;
} motor_config_t;

static motor_config_t motors[2];

void motor_init(void)
{
    motors[Left_Motor].pwm_pin = Left_Motor_PWM_Pin;
    motors[Left_Motor].dir1_pin = Left_Motor_Dir1_Pin;
    motors[Left_Motor].dir2_pin = Left_Motor_Dir2_Pin;

    motors[Right_Motor].pwm_pin = Right_Motor_PWM_Pin;
    motors[Right_Motor].dir1_pin = Right_Motor_Dir1_Pin;
    motors[Right_Motor].dir2_pin = Right_Motor_Dir2_Pin;

    for (int i=0; i<2; i++){
        motor_config_t *m &motors[i];

        gpio_set_function(m->pwm_pin, GPIO_FUNC_PWM);
        m->slice_num=pwm_gpio_to_slice_num(m->pwm_pin);
        m->channel=pwm_gpio_to_channel(m->pwm_pin);

        pwm_set_wrap(m->slice_num, PWM_MAX_VALUE);
        pwm_set_chan_level(m->slice_num,m->channel, 0);
        pwm_set_enabled(m->slice_num, true);

        gpio_init(m->dir1_pin);
        gpio_set_dir(m->dir1_pin, true); //true to out
        gpio_put(m->dir1_pin, 0);

        gpio_init(m->dir2_pin);
        gpio_set_dir(m->dir2_pin, true); //true to out
        gpio_put(m->dir2_pin, 0);
    }
}

void motor_set(motor_id_t motor, direction_t direction, uint8_t speed)
{
    if (motor>2) return;

    if (speed>100) speed=100;

    uint16_t pwm_value=(speed*PWM_MAX_VALUE)/100;
    motor_config_t *m &motors[motor];

    switch(direction){
        case Forward:
            gpio_put(m->dir1_pin, 1);
            gpio_put(m->dir2_pin, 0);
            pwm_set_chan_level(m->slice_num, m->channel, pwm_value);
            break;
        case Backward:
            gpio_put(m->dir1_pin, 0);
            gpio_put(m->dir2_pin, 1);
            pwm_set_chan_level(m->slice_num, m->channel, pwm_value);
            break;
        case Stop:    
            gpio_put(m->dir1_pin, 0);
            gpio_put(m->dir2_pin, 0);
            pwm_set_chan_level(m->slice_num, m->channel, 0);
            break;
    }
}

void motor_stop(motor_id_t motor)
{
    motor_set(motor, Stop, 0);
}

void all_motor_stop(void)
{   
    motor_stop(Left_Motor);
    motor_stop(Right_Motor);
}
