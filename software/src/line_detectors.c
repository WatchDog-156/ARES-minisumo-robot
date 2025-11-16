#include "inc/line_detectors.h"

void lineDetectorsInit(){
    adc_init();

    adc_gpio_init(LeftDetector);
    adc_gpio_init(RightDetector);
}


bool checkLineDetectors(){
    bool lineDetected = false;

    adc_select_input(LeftInput);
    uint16_t left_value = adc_read();
    if (left_value > LineDetectionThreshold){
        printf("Left line detector activated\n");
        lineDetected = true;
    }

    adc_select_input(RightInput);
    uint16_t right_value = adc_read();
    if (right_value > LineDetectionThreshold){
        printf("Right line detector activated\n");
        lineDetected = true;
    }

    return lineDetected;
}