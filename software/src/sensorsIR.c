#include "sensorsIR.h"


bool sensors_IR_init() {
    // Inicjalizacja I2C
    i2c_init(I2C_PORT 400 * 1000); // 400 kHz
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SCL);
    gpio_pull_up(I2C_SDA);

    // Inicjalizacja pinów XSHUT i wyłączenie wszystkich czujników
    for (int i = 0; i < NUM_SENSORS; i++) {
        gpio_init(xshut_pins[i]);
        gpio_set_dir(xshut_pins[i], GPIO_OUT);
        gpio_put(xshut_pins[i], 0);
    }
    sleep_ms(20);  // Czekaj na wyłączenie czujników

    // Inicjalizacja czujników jeden po drugim, zmiana adresów
    for (int i = NUM_SENSORS-1; i > 0; i--) {
        gpio_put(xshut_pins[i], 1); 
        sleep_ms(10); // Czekaj na uruchomienie czujnika
        uint8_t new_address = DEFAULT_VL53L0X_ADDR + i;

        // 0x8A to rejestr zmiany adresu I2C w VL53L0X
        int result = i2c_write_blocking(I2C_PORT, 0x8A, new_address, 2, false);
        if(result ==2){
            printf("Poprawnie zmieniono adres czujnika %d.\n", i);
        }else{
            printf("Błąd podczas zmiany adresu czujnika %d!\n", i);
            return false;
        }
    }
    gpio_put(xshut_pins[0], 1); 
    sleep_ms(10); // Czekaj na uruchomienie czujnika
    return true;
}


int16_t read_IR_distance_mm(uint8_t sensor_index) {
    if (sensor_index >= NUM_SENSORS) {
        return -1; // Błędny indeks
    }

    uint8_t address = DEFAULT_VL53L0X_ADDR + sensor_index;
    int distance = i2c_read_blocking(I2C_PORT, address, );







    if (measure.RangeStatus == 0) { // 0 = poprawny odczyt
        distance = measure.RangeMilliMeter;
    } else {
        distance = 8190; // Wartość "poza zakresem" dla VL53L0X
    }

    return distance;
}