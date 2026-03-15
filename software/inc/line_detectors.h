#ifndef LINE_DETECTORS_H
#define LINE_DETECTORS_H

#include <stdio.h>
#include "hardware/adc.h"
#include "hardware/gpio.h"

#define Left_Detector_Pin 28
#define Left_adc 2
#define Right_Detector_Pin 26 
#define Right_adc 0
#define Threshold 2900

typedef enum{
    Left,
    Right, 
    Both,
    None
} line_detector_t;

void line_detectors_init();

int read_detector_value(line_detector_t detector);

line_detector_t check_line_detectors();

#endif