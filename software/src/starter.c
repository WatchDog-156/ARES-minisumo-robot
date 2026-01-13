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

void starter_init(uint Start_pin, uint Kill_pin){
    gpio_init(Start_pin);
    gpio_set_dir(Start_pin, false); //false for in
    //gpio_pull_down(Start_pin); //tu zalezy od modulu

    gpio_init(Kill_pin);
    gpio_set_dir(Kill_pin, false); //false for in
    //gpio_pull_down(Kill_pin); //tu zalezy od modulu

    gpio_set_irq_enabled_with_callback(Start_pin, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled(Kill_pin, GPIO_IRQ_EDGE_RISE, true);
}

void changeState(State newState){
    current_state = newState;
}


State getState(void){
    return current_state;
}

//do controlera:
/*starter_init(Start_pin, Kill_pin);

while(true){
    State state = getState();

    switch (state){
        case Start:
            //czekanie na sygnal
            break;
        
        case Fighting:
            //walka
            break;

        case End:
            //Stop
            while(1)
            break;l

        }
    }*/