#ifndef SERWO_H
#define SERWO_H

#include "pico/stdlib.h"
#include <stdint.h>

#define LEFT_Serwo_PIN 4
#define RIGHT_Serwo_PIN 5

typedef enum
{
    LEFT_SERWO,
    RIGHT_SERWO
} serwo_id_t;

void serwo_init(void);
void serwo_set_posiotion(serwo_id_t serwo, uint8_t angle);
void serwo_set_pulse_us(serwo_id_t serwo, uint16_t perios_us);
void serwo_disabled(serwo_id_t serwo);


//frequency 50Hz, 0 degree - 1.5ms, 90 - 2ms, -90 - 1ms

#endif 