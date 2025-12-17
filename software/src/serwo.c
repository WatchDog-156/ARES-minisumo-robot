#include "serwo.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define SERWO_FREQ 50
#define SERWO_PERIOD_US 20000 

//frequency 50Hz, 0 degree - 1.5ms, 90 - 2ms, -90 - 1ms

typedef struct serwo
{
    uint pin;
    uint slice_num;
    uint channel;
} serwo_config_t;

static serwo_config_t serwos[2];

void serwo_init(void){
    serwos[LEFT_SERWO].pin = LEFT_Serwo_PIN;
    serwos[RIGHT_SERWO].pin = RIGHT_Serwo_PIN;

    for (int i=0; i<2; i++){
        serwo_config_t *s = &serwos[i];

        gpio_set_function(s->pin, GPIO_FUNC_PWM);
        s->slice_num = pwm_gpio_to_slice_num(s->pin);
        s->channel = pwm_gpio_to_channel(s->pin);

        //max_PWM_value = clock_freq/(divider*serwo_freq) - 1
        uint32_t clock_freq = clock_get_hz(clk_sys);
        uint32_t divider = 64;
        uint32_t MAX_PWM_VALUE = clock_freq/(divider*SERWO_FREQ) - 1;

        pwm_set_clkdiv(s->slice_num, divider);
        pwm_set_wrap(s->slice_num, MAX_PWM_VALUE);

        serwo_set_posiotion(i, 90);
        pwm_set_enabled(s->slice_num, true);
    }
}

void serwo_set_pulse_us(serwo_id_t serwo, uint16_t pulse_us){
    if (serwo>1) return;
    if (pulse_us<500) pulse_us = 500;
    if (pulse_us>2500) pulse_us = 2500;
    
    serwo_config_t *s = &serwos[serwo];

    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t divider = 64;
    uint32_t MAX_PWM_VALUE = clock_freq/(divider*SERWO_FREQ) - 1;

    uint32_t level = pulse_us*(MAX_PWM_VALUE+1)/SERWO_PERIOD_US;

    pwm_set_chan_level(s->slice_num, s->channel, level);

}

void serwo_set_posiotion(serwo_id_t serwo, uint8_t angle){
    if (angle>180) angle = 180;
    // if (angle<(-180)) angle = -180;

    //0 degree = 500us, 90 = 1500us, 180 = 2500 us
    uint16_t pulse_us = 500 + (angle * 2000)/180;
    serwo_set_pulse_us(serwo, pulse_us);
}

void serwo_disabled(serwo_id_t serwo){
    if (serwo > 1) return;

    serwo_config_t *s = &serwos[serwo];
    pwm_set_chan_level(s->slice_num, s->channel, 0);
}