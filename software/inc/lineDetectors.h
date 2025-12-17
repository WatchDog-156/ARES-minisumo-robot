#ifndef LINEDETECTORS.H
#define LINEDETECTORS.H

#include <stdio.h>
#include "hardware/adc.h"
#include "hardware/gpio.h"

#define LEFT_DETECTOR 27
#define LEFT_INPUT 1
#define RIGHT_DETECTOR 26 
#define RIGHT_INPUT 0
#define THRESHOLD 2000

/**
 * @brief Inicjalizuje czujniki linii.
 */
void line_detectors_init();

/**
 * @brief Sprawdza, czy którykolwiek z detektorów linii wykrywa linię.
 * @return true jeśli linia jest wykryta przez którykolwiek detektor, false w przeciwnym razie.
 */
bool check_line_detectors();

#endif