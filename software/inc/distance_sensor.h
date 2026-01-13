#ifndef DISTANCE_SENSOR_H
#define DISTANCE_SENSOR_H

#include <stdint.h>

#define I2C_SDA_PIN 20
#define I2C_SCL_PIN 21 
#define XSHUT1_PIN 16
#define XSHUT2_PIN 17
#define XSHUT3_PIN 18
#define XSHUT4_PIN 19

extern uint8_t i2cAddress;
void IR_init(void);
int tofInit(int sensor_index, int Addr, int boolLongRange);
int tofReadDistance(int sensor_index);
void tofSetAddress(uint8_t oldAddr, uint8_t newAddr);
void tofSetCurrentAddress(uint8_t addr);
void tofSetCurrentSensor(int sensor_index);

#endif