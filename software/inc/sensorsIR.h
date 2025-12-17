#ifndef SENSORSIR_H
#define SENSORSIR_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define I2C_SDA 20
#define I2C_SCL 21

#define NUM_SENSORS 4
uint xshut_pins[NUM_SENSORS] = {10, 11, 12, 13};
#define DEFAULT_VL53L0X_ADDR 0x29

/**
 * @brief Inicjalizuje czujniki IR VL53L0X oraz zmienia adresy czujników.
 *        Umożliwia to korzystanie z 4 czujników na jednej magistrali I2C.
 *        Ustawia adresy czujników na 0x29, 0x2A, 0x2B, 0x2C.
 * @return true jeśli inicjalizacja powiodła się, false w przeciwnym razie
 */
bool sensors_IR_init();

 /**
  * @brief Odczytuje odległość z konkretnego czujnika.
  * @param sensor_index Indeks czujnika (0 do 3)
  * @return int16_t Odległość w milimetrach lub -1 w przypadku błędu
  */
int16_t read_IR_distance_mm(uint8_t sensor_index);

#endif // SENSORSIR_H