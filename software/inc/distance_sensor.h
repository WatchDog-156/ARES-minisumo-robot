#ifndef DISTANCE_SENSOR.H
#define DISTANCE_SENSOR.H

#include <stdint.h>

#define I2C_SDA_PIN 20
#define I2C_SCL_PIN 21
#define XSHUT1_PIN 16
#define XSHUT2_PIN 17
#define XSHUT3_PIN 18
#define XSHUT4_PIN 19

void IR_init(void);
int tofInit(int Chan, int Addr, int boolLongRange);
int tofReadDistance(void);
void tofSetAddress(uint8_t oldAddr, uint8_t newwAddr);


#endif