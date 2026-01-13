#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "starter.h"

static volatile State current_state = Start;

void gpio_callback(uint gpio, uint32_t events){
    if(gpio == Kill_pin){
        current_state = End;
    }
    else if(gpio == Start_pin && current_state == Start){
        current_state = Fighting;
    }
}

void starter_init(){
    gpio_init(Start_pin);
    gpio_set_dir(Start_pin, false);
    //gpio_pull_down(Start_pin); //tu zalezy od modulu

    gpio_init(Kill_pin);
    gpio_set_dir(Kill_pin, false); 
    //gpio_pull_down(Kill_pin); //tu zalezy od modulu

    gpio_set_irq_enabled_with_callback(Start_pin, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled(Kill_pin, GPIO_IRQ_EDGE_RISE, true);
}

void changeState(State newState){
    current_state = newState;
}


State getState(){
    return current_state;
}