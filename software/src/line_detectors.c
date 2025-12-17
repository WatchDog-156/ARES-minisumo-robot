#include "line_detectors.h"

void line_detectors_init(){
    adc_init();

    adc_gpio_init(Left_Detector_Pin);
    adc_gpio_init(Right_Detector_Pin);
}

int read_detector_value(line_detector_t detector){
    if (detector==Left)
        adc_select_input(Left_adc);
    else if(detector==Right)
        adc_select_input(Right_adc);
    else 
        return -1;

    return adc_read();
}


line_detector_t check_line_detectors(){
    line_detector_t lineDetected = None;

    int left_value = read_detector_value(Left);
    if (left_value > Threshold)
        lineDetected = Left;

    int right_value = read_detector_value(Right);
    if (right_value > Threshold){
        if (lineDetected==Left)
            lineDetected = Both;
        else
            lineDetected = Right;
    }

    return lineDetected;
}