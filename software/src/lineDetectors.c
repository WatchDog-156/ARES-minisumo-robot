#include "lineDetectors.h"

void line_detectors_init(){
    adc_init();

    adc_gpio_init(LEFT_DETECTOR);
    adc_gpio_init(RIGHT_DETECTOR);
}


bool check_line_detectors(){
    bool lineDetected = false;

    // Odczyt lewego detektora
    adc_select_input(LEFT_INPUT);
    uint16_t left_value = adc_read();
    if (left_value > THRESHOLD){
        printf("Left line detector activated\n");
        lineDetected = true;
    }
    printf("Left detector value: %d\n", left_value);

    // Odczyt prawego detektora
    adc_select_input(RIGHT_INPUT);
    uint16_t right_value = adc_read();
    if (right_value > THRESHOLD){
        printf("Right line detector activated\n");
        lineDetected = true;
    }
    printf("Right detector value: %d\n", right_value);

    return lineDetected;
}