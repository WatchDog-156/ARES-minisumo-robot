#ifndef LINE_DETECTORS.H
#define LINE_DETECTORS.H

#include "hardware/adc.h"
#include "hardware/gpio.h"

#define LeftDetector 27
#define LeftInput 1
#define RightDetector 26 
#define RightInput 0
#define LineDetectionThreshold 2000

void lineDetectorsInit();

bool checkLineDetectors();

#endif