#include "distance_sensor.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define FREQ 100000


uint8_t i2cAddress= 0x29;

static unsigned char stop_variable;
static uint32_t measurement_timing_budget_us;

// Deklaracje wewnetrzne
static unsigned char readReg(unsigned char ucAddr);
static unsigned short readReg16(unsigned char ucAddr);
static void writeReg16(unsigned char ucAddr, unsigned short usValue);
static void writeReg(unsigned char ucAddr, unsigned char ucValue);
static void writeRegList(unsigned char *ucList);
static int initSensor(int);
static int performSingleRefCalibration(uint8_t vhv_init_byte);
static int setMeasurementTimingBudget(uint32_t budget_us);
static int setVcselPulsePeriod(int type, uint8_t period_pclks);

#define calcMacroPeriod(vcsel_period_pclks) ((((uint32_t)2304 * (vcsel_period_pclks) * 1655) + 500) / 1000)
#define encodeVcselPeriod(period_pclks) (((period_pclks) >> 1) - 1)

#define SEQUENCE_ENABLE_FINAL_RANGE 0x80
#define SEQUENCE_ENABLE_PRE_RANGE   0x40
#define SEQUENCE_ENABLE_TCC         0x10
#define SEQUENCE_ENABLE_DSS         0x08
#define SEQUENCE_ENABLE_MSRC        0x04

// typedef enum vcselperiodtype { VcselPeriodPreRange, VcselPeriodFinalRange } vcselPeriodType;

typedef struct {
      uint16_t pre_range_vcsel_period_pclks, final_range_vcsel_period_pclks;
      uint16_t msrc_dss_tcc_mclks, pre_range_mclks, final_range_mclks;
      uint32_t msrc_dss_tcc_us, pre_range_us, final_range_us;
} SequenceStepTimeouts;

// VL53L0X Rejestry
#define REG_SYSRANGE_START			            0x00
#define REG_RESULT_INTERRUPT_STATUS 		    0x13
#define RESULT_RANGE_STATUS      		        0x14
#define PRE_RANGE_CONFIG_VCSEL_PERIOD           0x50
#define PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI      0x51
#define PRE_RANGE_CONFIG_VALID_PHASE_LOW        0x56
#define PRE_RANGE_CONFIG_VALID_PHASE_HIGH       0x57
#define REG_MSRC_CONFIG_CONTROL                 0x60
#define FINAL_RANGE_CONFIG_VCSEL_PERIOD         0x70
#define FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI    0x71
#define MSRC_CONFIG_TIMEOUT_MACROP              0x46
#define FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT  0x44
#define SYSTEM_SEQUENCE_CONFIG                  0x01
#define SYSTEM_INTERRUPT_CONFIG_GPIO            0x0A
#define VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV       0x89
#define REG_I2C_SLAVE_DEVICE_ADDRESS            0x8A
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_0        0xB0
#define GPIO_HV_MUX_ACTIVE_HIGH                 0x84
#define SYSTEM_INTERRUPT_CLEAR                  0x0B
#define I2C_ADDRESS_MASK                        0x7F

// Tablice inicjalizacyjne (magic numbers)
unsigned char I2CMode[] = {4, 0x88,0x00, 0x80,0x01, 0xff,0x01, 0x00,0x00};
unsigned char I2CMode2[] = {3, 0x00,0x01, 0xff,0x00, 0x80,0x00};
unsigned char SPAD0[] = {4, 0x80,0x01, 0xff,0x01, 0x00,0x00, 0xff,0x06};
unsigned char SPAD1[] = {5, 0xff,0x07, 0x81,0x01, 0x80,0x01, 0x94,0x6b, 0x83,0x00};
unsigned char SPAD2[] = {4, 0xff,0x01, 0x00,0x01, 0xff,0x00, 0x80,0x00};
unsigned char SPAD[] = {5, 0xff,0x01, 0x4f,0x00, 0x4e,0x2c, 0xff,0x00, 0xb6,0xb4};
unsigned char DefTuning[] = {80, 0xff,0x01, 0x00,0x00, 0xff,0x00, 0x09,0x00,
0x10,0x00, 0x11,0x00, 0x24,0x01, 0x25,0xff, 0x75,0x00, 0xff,0x01, 0x4e,0x2c,
0x48,0x00, 0x30,0x20, 0xff,0x00, 0x30,0x09, 0x54,0x00, 0x31,0x04, 0x32,0x03,
0x40,0x83, 0x46,0x25, 0x60,0x00, 0x27,0x00, 0x50,0x06, 0x51,0x00, 0x52,0x96,
0x56,0x08, 0x57,0x30, 0x61,0x00, 0x62,0x00, 0x64,0x00, 0x65,0x00, 0x66,0xa0,
0xff,0x01, 0x22,0x32, 0x47,0x14, 0x49,0xff, 0x4a,0x00, 0xff,0x00, 0x7a,0x0a,
0x7b,0x00, 0x78,0x21, 0xff,0x01, 0x23,0x34, 0x42,0x00, 0x44,0xff, 0x45,0x26,
0x46,0x05, 0x40,0x40, 0x0e,0x06, 0x20,0x1a, 0x43,0x40, 0xff,0x00, 0x34,0x03,
0x35,0x44, 0xff,0x01, 0x31,0x04, 0x4b,0x09, 0x4c,0x05, 0x4d,0x04, 0xff,0x00,
0x44,0x00, 0x45,0x20, 0x47,0x08, 0x48,0x28, 0x67,0x00, 0x70,0x04, 0x71,0x01,
0x72,0xfe, 0x76,0x00, 0x77,0x00, 0xff,0x01, 0x0d,0x01, 0xff,0x00, 0x80,0x01,
0x01,0xf8, 0xff,0x01, 0x8e,0x01, 0x00,0x01, 0xff,0x00, 0x80,0x00};

//Funkcje I2C PICO

static void writeReg(unsigned char Addr, unsigned char Value){
    uint8_t Temp[2] = {Addr, Value};
    i2c_write_blocking(i2c_default, i2cAddress, Temp, 2, true);
}
static void writeReg16(unsigned char Addr, unsigned short Value){
    uint8_t Temp[3] = {Addr, (uint8_t)(Value >> 8), (uint8_t)Value};
    i2c_write_blocking(i2c_default, i2cAddress, Temp, 3, true);
}
static unsigned char readReg(unsigned char Addr){
    unsigned char Temp;
    i2c_write_blocking(i2c_default,i2cAddress, &Addr, 1, true);
    i2c_read_blocking(i2c_default, i2cAddress, &Temp, 1, false);
    return Temp;
}
static unsigned short readReg16(unsigned char Addr){
    unsigned char Temp[2];
    i2c_write_blocking(i2c_default, i2cAddress, &Addr, 1, true);
    i2c_read_blocking(i2c_default, i2cAddress, Temp, 2, false);
    return (unsigned short)((Temp[0] << 8) + Temp[1]);
}
static void writeRegList(unsigned char *List){
    unsigned char Count = *List++;
    while(Count){
        i2c_write_blocking(i2c_default, i2cAddress, List, 2, false);
        List+=2;
        Count--;
    }
}
void tofSetAddress(uint8_t oldAddr, uint8_t newAddr){
    uint8_t  Temp[2] = {REG_I2C_SLAVE_DEVICE_ADDRESS, (uint8_t)(newAddr & I2C_ADDRESS_MASK)};
    i2c_write_blocking(i2c_default, oldAddr, Temp, 2, false);
    i2cAddress = newAddr;
}
static int getSpadInfo(unsigned char *Count, unsigned char *TypeIsAperture){
    unsigned char Temp;
    int Timeout = 0;
    #define MAX_TIMEOUT 50
    writeRegList(SPAD0);
    writeReg(0x83, readReg(0x83) | 0x04);
    writeRegList(SPAD1);
    while(Timeout < MAX_TIMEOUT)
    {
        if (readReg(0x83) != 0x00) break;
        Timeout++;
        sleep_us(5000);
    }
    writeReg(0x83,0x01);
    Temp = readReg(0x92);
    *Count = (Temp & I2C_ADDRESS_MASK);
    *TypeIsAperture = (Temp & 0x80);
    writeReg(0x81,0x00);
    writeReg(0xff,0x06);
    writeReg(0x83, readReg(0x83) & ~0x04);
    writeRegList(SPAD2);
    return 1;
}
static uint16_t decodeTimeout(uint16_t reg_val){
    return (uint16_t)((reg_val & 0x00FF) << (uint16_t)((reg_val & 0xFF00) >> 8)) + 1;
}
static uint32_t timeoutMclksToMicroseconds(uint16_t timeout_period_mclks, uint8_t vcsel_period_pclks){
    uint32_t macro_period_ns = calcMacroPeriod(vcsel_period_pclks);
    return ((timeout_period_mclks * macro_period_ns) + (macro_period_ns / 2)) / 1000;
}
static uint16_t encodeTimeout(uint16_t timeout_mclks){
    uint32_t ls_byte = 0;
    uint16_t ms_byte = 0;
    if (timeout_mclks > 0){
        ls_byte = timeout_mclks - 1;
        while ((ls_byte & 0xFFFFFF00) > 0){
            ls_byte >>= 1;
            ms_byte++;
        }
        return (ms_byte << 8) | (ls_byte & 0xFF);
    }
    return 0;
}

static int performSingleRefCalibration(uint8_t vhv_init_byte)
{
    int Timeout = 0;
    writeReg(REG_SYSRANGE_START, 0x01 | vhv_init_byte);
    while (((readReg(REG_RESULT_INTERRUPT_STATUS) & 0x07) == 0) && (Timeout < 100))
    {
        Timeout++;
        sleep_us(5000);
    }
    writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01);
    writeReg(REG_SYSRANGE_START, 0x00);
    return 1;
}
static int initSensor(int boolLongRange){
    unsigned char spad_count=0, spad_type_is_aperture=0, ref_spad_map[6];

    // set 2.8V mode
    writeReg(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, readReg(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV) | 0x01); // set bit 0

    // Set I2C standard mode
    writeRegList(I2CMode);
    stop_variable = readReg(0x91);
    writeRegList(I2CMode2);

    // disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks
    writeReg(REG_MSRC_CONFIG_CONTROL, readReg(REG_MSRC_CONFIG_CONTROL) | 0x12);

    // Q9.7 fixed point format (9 integer bits, 7 fractional bits)
    writeReg16(FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, boolLongRange ? 13 : 32); // 0.25
    writeReg(SYSTEM_SEQUENCE_CONFIG, 0xFF);
    getSpadInfo(&spad_count, &spad_type_is_aperture);

    // SPAD Map
    uint8_t start_addr = GLOBAL_CONFIG_SPAD_ENABLES_REF_0;
    i2c_write_blocking(i2c_default, i2cAddress, &start_addr, 1, true);
    i2c_read_blocking(i2c_default, i2cAddress, ref_spad_map, 6, false);
    writeRegList(SPAD);

    uint8_t first = (spad_type_is_aperture) ? 12 : 0;
    uint8_t enabled = 0;
    // clear bits for unused SPADs
    for (int i=0; i<48; i++){
        if (i < first || enabled == spad_count){
            ref_spad_map[i>>3] &= ~(1<<(i & 7));
        } 
        else if (ref_spad_map[i>>3] & (1<< (i & 7))){
            enabled++;
        }
    }
    uint8_t spad_temp[7] = {GLOBAL_CONFIG_SPAD_ENABLES_REF_0};
    memcpy(&spad_temp[1], ref_spad_map, 6);
    i2c_write_blocking(i2c_default, i2cAddress, spad_temp, 7, false);

    writeRegList(DefTuning); // long list of magic numbers
    writeReg(SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);
    writeReg(GPIO_HV_MUX_ACTIVE_HIGH, readReg(GPIO_HV_MUX_ACTIVE_HIGH) & ~0x10); // active low
    writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01);
    writeReg(SYSTEM_SEQUENCE_CONFIG, 0xe8);    
    performSingleRefCalibration(0x40);
    performSingleRefCalibration(0x00);
    writeReg(SYSTEM_SEQUENCE_CONFIG, 0xe8);
    return 1;
}
int tofInit(int Chan, int Addr, int boolLongRange){
    i2cAddress = Addr;
    return initSensor(boolLongRange);
}
int tofReadDistance(void){
    int Timeout = 0;
    writeReg(0x80, 0x01);
    writeReg(0xFF, 0x01);
    writeReg(0x00, 0x00);
    writeReg(0x91, stop_variable);
    writeReg(0x00, 0x01);
    writeReg(0xFF, 0x00);
    writeReg(0x80, 0x00);

    writeReg(REG_SYSRANGE_START, 0x01);

    Timeout = 0;
    while ((readReg(REG_SYSRANGE_START) & 0x01) && Timeout < 50)
    {
        Timeout++;
        sleep_us(5000);
    }
    Timeout = 0;
    while (((readReg(REG_RESULT_INTERRUPT_STATUS) & 0x07) == 0) && Timeout < 50)
    {
        Timeout++;
        sleep_us(5000);
    }
    int range = readReg16(RESULT_RANGE_STATUS + 10);
    writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01);
    return range;
}



void IR_init(void){
    i2c_init(i2c0, FREQ);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    //bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    int xshut[4] = {XSHUT1_PIN, XSHUT2_PIN, XSHUT3_PIN, XSHUT4_PIN};
    uint8_t addr[4] = {0x31, 0x32, 0x33, 0x34}

    for(int i=0; i<4; i++){
        gpio_init(xshut[i]);
        gpio_set_dir(xshut[i], GPIO_OUT);
        gpio_put(xshut[i], 0);
    }
    sleep_ms(10);

    for(int i=0; i<4; i++){
        gpio_put(xshut[i], 1);
        sleep_ms(10);
        tofSetAddress(0x29, addr[i]);
        tofInit(0, addr[i], 0);
    }
}

