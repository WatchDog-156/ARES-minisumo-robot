#include "distance_sensor.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define FREQ 100000

// Zmienne globalne biblioteki
uint8_t i2cAddress = 0x29;
static unsigned char stop_variable[4];
static uint32_t measurement_timing_budget_us;
static int current_sensor_index = -1;
 
static int performSingleRefCalibration(uint8_t vhv_init_byte);
static int setMeasurementTimingBudget(uint32_t budget_us);
// --- Makra pomocnicze ---
#define calcMacroPeriod(vcsel_period_pclks) ((((uint32_t)2304 * (vcsel_period_pclks) * 1655) + 500) / 1000)
#define encodeVcselPeriod(period_pclks) (((period_pclks) >> 1) - 1)

#define SEQUENCE_ENABLE_FINAL_RANGE 0x80
#define SEQUENCE_ENABLE_PRE_RANGE   0x40
#define SEQUENCE_ENABLE_TCC         0x10
#define SEQUENCE_ENABLE_DSS         0x08
#define SEQUENCE_ENABLE_MSRC        0x04

typedef enum vcselperiodtype { VcselPeriodPreRange, VcselPeriodFinalRange } vcselPeriodType;
static int setVcselPulsePeriod(vcselPeriodType type, uint8_t period_pclks);

typedef struct tagSequenceStepTimeouts
    {
      uint16_t pre_range_vcsel_period_pclks, final_range_vcsel_period_pclks;

      uint16_t msrc_dss_tcc_mclks, pre_range_mclks, final_range_mclks;
      uint32_t msrc_dss_tcc_us,    pre_range_us,    final_range_us;
    } SequenceStepTimeouts;

// --- Rejestry ---
#define REG_SYSRANGE_START                      0x00
#define REG_RESULT_INTERRUPT_STATUS             0x13
#define RESULT_RANGE_STATUS                     0x14
#define ALGO_PHASECAL_LIM                       0x30
#define ALGO_PHASECAL_CONFIG_TIMEOUT            0x30
#define GLOBAL_CONFIG_VCSEL_WIDTH               0x32
#define FINAL_RANGE_CONFIG_VALID_PHASE_LOW      0x47
#define FINAL_RANGE_CONFIG_VALID_PHASE_HIGH     0x48
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


// --- Tablice "Magic Numbers" ---
unsigned char I2CMode[] = {4, 0x88,0x00, 0x80,0x01, 0xff,0x01, 0x00,0x00};
unsigned char I2CMode2[] = {3, 0x00,0x01, 0xff,0x00, 0x80,0x00};
unsigned char SPAD0[] = {4, 0x80,0x01, 0xff,0x01, 0x00,0x00, 0xff,0x06};
unsigned char SPAD1[] = {5, 0xff,0x07, 0x81,0x01, 0x80,0x01, 0x94,0x6b, 0x83,0x00};
unsigned char SPAD2[] = {4, 0xff,0x01, 0x00,0x01, 0xff,0x00, 0x80,0x00};
unsigned char SPAD_MAP_SETUP[] = {5, 0xff,0x01, 0x4f,0x00, 0x4e,0x2c, 0xff,0x00, 0xb6,0xb4};
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



static void readMulti(unsigned char Addr, unsigned char *Buf, int Count){
    i2c_write_blocking(i2c_default, i2cAddress, &Addr, 1, true);
    i2c_read_blocking(i2c_default, i2cAddress, Buf, Count, false);
}

static void writeMulti(unsigned char Addr, unsigned char *Buf, int Count){
    unsigned char Temp[16];
    i2c_write_blocking(i2c_default, i2cAddress, Temp, Count+1, false);
}

// --- Funkcje I2C ---
static int writeReg(unsigned char Addr, unsigned char Value){
    uint8_t Temp[2] = {Addr, Value};
    int result = i2c_write_blocking(i2c_default, i2cAddress, Temp, 2, false);
    if (result != 2){
        printf("I2C Write Blad: addr=0x%02X, reg=0x%02X, zapisano=%d/2\n", i2cAddress, Addr, result);
        return 0;
    }
    return 1;
}
static int writeReg16(unsigned char Addr, unsigned short Value){
    uint8_t Temp[3] = {Addr, (uint8_t)(Value >> 8), (uint8_t)Value};
    int result = i2c_write_blocking(i2c_default, i2cAddress, Temp, 3, false);
    if (result != 3){
        printf("I2C Write Blad: addr=0x%02X, reg=0x%02X, zapisano=%d/3\n", i2cAddress, Addr, result);
        return 0;
    }
    return 1;
}
static unsigned char readReg(unsigned char Addr){
    unsigned char Temp;
    int result = i2c_write_blocking(i2c_default, i2cAddress, &Addr, 1, true);
    if (result != 1){
        printf("I2C Read Blad (faza zapisywania): addr=0x%02X, reg=0x%02Xn", i2cAddress, Addr);
        return 0xFF;
    }
    result = i2c_read_blocking(i2c_default, i2cAddress, &Temp, 1, false);
    if (result != 1){
        printf("I2C Read Blad (faza czytania): addr=0x%02X, reg=0x%02Xn", i2cAddress, Addr);
        return 0xFF;
    }
    return Temp;
}
static unsigned short readReg16(unsigned char Addr){
    unsigned char Temp[2];
    int result = i2c_write_blocking(i2c_default, i2cAddress, &Addr, 1, true);
    if (result!=1) return  0xFFFF;
    result = i2c_read_blocking(i2c_default, i2cAddress, Temp, 2, false);
    if (result!=2) return  0xFFFF;
    return (unsigned short)((Temp[0] << 8) + Temp[1]);
}
static void writeRegList(unsigned char *List){
    unsigned char Count = *List++;
    while(Count){
        i2c_write_blocking(i2c_default, i2cAddress, List, 2, false);
        List += 2;
        Count--;
    }
}

static int getSpadInfo(unsigned char *Count, unsigned char *TypeIsAperture){
    int Timeout;
    unsigned char Temp;
    #define MAX_TIMEOUT 50

    writeRegList(SPAD0);
    writeReg(0x83, readReg(0x83) | 0x04);
    writeRegList(SPAD1);
    Timeout = 0;
    while(Timeout < MAX_TIMEOUT)
    {
        if (readReg(0x83) != 0x00) break;
        Timeout++;
        sleep_us(5000);
    }
    if (Timeout == MAX_TIMEOUT)
    {
        fprintf(stderr, "Timeout while waiting for SPAD info\n");
        return 0;
    }
    writeReg(0x83,0x01);
    Temp = readReg(0x92);
    *Count = (Temp & 0x7f);
    *TypeIsAperture = (Temp & 0x80);
    writeReg(0x81,0x00);
    writeReg(0xff,0x06);
    writeReg(0x83, readReg(0x83) & ~0x04);
    writeRegList(SPAD2);
    
  return 1;
}

// --- Funkcje Obliczeniowe Biblioteki ---
static uint16_t decodeTimeout(uint16_t reg_val){
    return (uint16_t)((reg_val & 0x00FF) << (uint16_t)((reg_val & 0xFF00) >> 8)) + 1;
}

static uint32_t timeoutMclksToMicroseconds(uint16_t timeout_period_mclks, uint8_t vcsel_period_pclks){
    uint32_t macro_period_ns = calcMacroPeriod(vcsel_period_pclks);
    return ((timeout_period_mclks * macro_period_ns) + (macro_period_ns / 2)) / 1000;
}

static uint32_t timeoutMicrosecondsToMclks(uint32_t timeout_period_us, uint8_t vcsel_period_pclks){
    uint32_t macro_period_ns = calcMacroPeriod(vcsel_period_pclks);
    return (((timeout_period_us * 1000) + (macro_period_ns / 2)) / macro_period_ns);
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

static void getSequenceStepTimeouts(uint8_t enables, SequenceStepTimeouts * timeouts){
    timeouts->pre_range_vcsel_period_pclks = ((readReg(PRE_RANGE_CONFIG_VCSEL_PERIOD) + 1) << 1);
    timeouts->msrc_dss_tcc_mclks = readReg(MSRC_CONFIG_TIMEOUT_MACROP) + 1;
    timeouts->msrc_dss_tcc_us = timeoutMclksToMicroseconds(timeouts->msrc_dss_tcc_mclks, timeouts->pre_range_vcsel_period_pclks);
    timeouts->pre_range_mclks = decodeTimeout(readReg16(PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI));
    timeouts->pre_range_us = timeoutMclksToMicroseconds(timeouts->pre_range_mclks, timeouts->pre_range_vcsel_period_pclks);
    timeouts->final_range_vcsel_period_pclks = ((readReg(FINAL_RANGE_CONFIG_VCSEL_PERIOD) + 1) << 1);
    timeouts->final_range_mclks = decodeTimeout(readReg16(FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI));

    if (enables & SEQUENCE_ENABLE_PRE_RANGE){
        timeouts->final_range_mclks -= timeouts->pre_range_mclks;
    }
    timeouts->final_range_us = timeoutMclksToMicroseconds(timeouts->final_range_mclks, timeouts->final_range_vcsel_period_pclks);
}


// Set the VCSEL (vertical cavity surface emitting laser) pulse period for the
// given period type (pre-range or final range) to the given value in PCLKs.
// Longer periods seem to increase the potential range of the sensor.
// Valid values are (even numbers only):
//  pre:  12 to 18 (initialized default: 14)
//  final: 8 to 14 (initialized default: 10)
// based on VL53L0X_set_vcsel_pulse_period()
static int setVcselPulsePeriod(vcselPeriodType type, uint8_t period_pclks)
{
  uint8_t vcsel_period_reg = encodeVcselPeriod(period_pclks);

  uint8_t enables;
  SequenceStepTimeouts timeouts;

  enables = readReg(SYSTEM_SEQUENCE_CONFIG);
  getSequenceStepTimeouts(enables, &timeouts);

  // "Apply specific settings for the requested clock period"
  // "Re-calculate and apply timeouts, in macro periods"

  // "When the VCSEL period for the pre or final range is changed,
  // the corresponding timeout must be read from the device using
  // the current VCSEL period, then the new VCSEL period can be
  // applied. The timeout then must be written back to the device
  // using the new VCSEL period.
  //
  // For the MSRC timeout, the same applies - this timeout being
  // dependant on the pre-range vcsel period."


  if (type == VcselPeriodPreRange)
  {
    // "Set phase check limits"
    switch (period_pclks)
    {
      case 12:
        writeReg(PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x18);
        break;

      case 14:
        writeReg(PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x30);
        break;

      case 16:
        writeReg(PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x40);
        break;

      case 18:
        writeReg(PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x50);
        break;

      default:
        // invalid period
        return 0;
    }
    writeReg(PRE_RANGE_CONFIG_VALID_PHASE_LOW, 0x08);

    // apply new VCSEL period
    writeReg(PRE_RANGE_CONFIG_VCSEL_PERIOD, vcsel_period_reg);

    // update timeouts

    // set_sequence_step_timeout() begin
    // (SequenceStepId == VL53L0X_SEQUENCESTEP_PRE_RANGE)

    uint16_t new_pre_range_timeout_mclks =
      timeoutMicrosecondsToMclks(timeouts.pre_range_us, period_pclks);

    writeReg16(PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI,
      encodeTimeout(new_pre_range_timeout_mclks));

    // set_sequence_step_timeout() end

    // set_sequence_step_timeout() begin
    // (SequenceStepId == VL53L0X_SEQUENCESTEP_MSRC)

    uint16_t new_msrc_timeout_mclks =
      timeoutMicrosecondsToMclks(timeouts.msrc_dss_tcc_us, period_pclks);

    writeReg(MSRC_CONFIG_TIMEOUT_MACROP,
      (new_msrc_timeout_mclks > 256) ? 255 : (new_msrc_timeout_mclks - 1));

    // set_sequence_step_timeout() end
  }
  else if (type == VcselPeriodFinalRange)
  {
    switch (period_pclks)
    {
      case 8:
        writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x10);
        writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
        writeReg(GLOBAL_CONFIG_VCSEL_WIDTH, 0x02);
        writeReg(ALGO_PHASECAL_CONFIG_TIMEOUT, 0x0C);
        writeReg(0xFF, 0x01);
        writeReg(ALGO_PHASECAL_LIM, 0x30);
        writeReg(0xFF, 0x00);
        break;

      case 10:
        writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x28);
        writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
        writeReg(GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
        writeReg(ALGO_PHASECAL_CONFIG_TIMEOUT, 0x09);
        writeReg(0xFF, 0x01);
        writeReg(ALGO_PHASECAL_LIM, 0x20);
        writeReg(0xFF, 0x00);
        break;

      case 12:
        writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x38);
        writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
        writeReg(GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
        writeReg(ALGO_PHASECAL_CONFIG_TIMEOUT, 0x08);
        writeReg(0xFF, 0x01);
        writeReg(ALGO_PHASECAL_LIM, 0x20);
        writeReg(0xFF, 0x00);
        break;

      case 14:
        writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x48);
        writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
        writeReg(GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
        writeReg(ALGO_PHASECAL_CONFIG_TIMEOUT, 0x07);
        writeReg(0xFF, 0x01);
        writeReg(ALGO_PHASECAL_LIM, 0x20);
        writeReg(0xFF, 0x00);
        break;

      default:
        // invalid period
        return 0;
    }

    // apply new VCSEL period
    writeReg(FINAL_RANGE_CONFIG_VCSEL_PERIOD, vcsel_period_reg);

    // update timeouts

    // set_sequence_step_timeout() begin
    // (SequenceStepId == VL53L0X_SEQUENCESTEP_FINAL_RANGE)

    // "For the final range timeout, the pre-range timeout
    //  must be added. To do this both final and pre-range
    //  timeouts must be expressed in macro periods MClks
    //  because they have different vcsel periods."

    uint16_t new_final_range_timeout_mclks =
      timeoutMicrosecondsToMclks(timeouts.final_range_us, period_pclks);

    if (enables & SEQUENCE_ENABLE_PRE_RANGE)
    {
      new_final_range_timeout_mclks += timeouts.pre_range_mclks;
    }

    writeReg16(FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI,
    encodeTimeout(new_final_range_timeout_mclks));

    // set_sequence_step_timeout end
  }
  else
  {
    // invalid type
    return 0;
  }

  // "Finally, the timing budget must be re-applied"

  setMeasurementTimingBudget(measurement_timing_budget_us);

  // "Perform the phase calibration. This is needed after changing on vcsel period."
  // VL53L0X_perform_phase_calibration() begin

  uint8_t sequence_config = readReg(SYSTEM_SEQUENCE_CONFIG);
  writeReg(SYSTEM_SEQUENCE_CONFIG, 0x02);
  performSingleRefCalibration(0x0);
  writeReg(SYSTEM_SEQUENCE_CONFIG, sequence_config);

  // VL53L0X_perform_phase_calibration() end

  return 1;
}

static int setMeasurementTimingBudget(uint32_t budget_us){
    uint8_t enables;
    SequenceStepTimeouts timeouts;
    uint16_t const StartOverhead = 1320; 
    uint16_t const EndOverhead = 960;
    uint16_t const MsrcOverhead = 660;
    uint16_t const TccOverhead = 590;
    uint16_t const DssOverhead = 690;
    uint16_t const PreRangeOverhead = 660;
    uint16_t const FinalRangeOverhead = 550;
    uint32_t const MinTimingBudget = 20000;

    if (budget_us < MinTimingBudget) return 0;
    uint32_t used_budget_us = StartOverhead + EndOverhead;
    enables = readReg(SYSTEM_SEQUENCE_CONFIG);
    getSequenceStepTimeouts(enables, &timeouts);

    if (enables & SEQUENCE_ENABLE_TCC) used_budget_us += (timeouts.msrc_dss_tcc_us + TccOverhead);
    if (enables & SEQUENCE_ENABLE_DSS) used_budget_us += 2 * (timeouts.msrc_dss_tcc_us + DssOverhead);
    else if (enables & SEQUENCE_ENABLE_MSRC) used_budget_us += (timeouts.msrc_dss_tcc_us + MsrcOverhead);
    if (enables & SEQUENCE_ENABLE_PRE_RANGE) used_budget_us += (timeouts.pre_range_us + PreRangeOverhead);

    if (enables & SEQUENCE_ENABLE_FINAL_RANGE){
        used_budget_us += FinalRangeOverhead;
        if (used_budget_us > budget_us) return 0;
        uint32_t final_range_timeout_us = budget_us - used_budget_us;
        uint16_t final_range_timeout_mclks = timeoutMicrosecondsToMclks(final_range_timeout_us, timeouts.final_range_vcsel_period_pclks);
        if (enables & SEQUENCE_ENABLE_PRE_RANGE) final_range_timeout_mclks += timeouts.pre_range_mclks;
        writeReg16(FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI, encodeTimeout(final_range_timeout_mclks));
        measurement_timing_budget_us = budget_us;
    }
    return 1;
}

static uint32_t getMeasurementTimingBudget(void)
{
  uint8_t enables;
  SequenceStepTimeouts timeouts;

  uint16_t const StartOverhead     = 1910; // note that this is different than the value in set_
  uint16_t const EndOverhead        = 960;
  uint16_t const MsrcOverhead       = 660;
  uint16_t const TccOverhead        = 590;
  uint16_t const DssOverhead        = 690;
  uint16_t const PreRangeOverhead   = 660;
  uint16_t const FinalRangeOverhead = 550;

  // "Start and end overhead times always present"
  uint32_t budget_us = StartOverhead + EndOverhead;

  enables = readReg(SYSTEM_SEQUENCE_CONFIG);
  getSequenceStepTimeouts(enables, &timeouts);

  if (enables & SEQUENCE_ENABLE_TCC)
  {
    budget_us += (timeouts.msrc_dss_tcc_us + TccOverhead);
  }

  if (enables & SEQUENCE_ENABLE_DSS)
  {
    budget_us += 2 * (timeouts.msrc_dss_tcc_us + DssOverhead);
  }
  else if (enables & SEQUENCE_ENABLE_MSRC)
  {
    budget_us += (timeouts.msrc_dss_tcc_us + MsrcOverhead);
  }

  if (enables & SEQUENCE_ENABLE_PRE_RANGE)
  {
    budget_us += (timeouts.pre_range_us + PreRangeOverhead);
  }

  if (enables & SEQUENCE_ENABLE_FINAL_RANGE)
  {
    budget_us += (timeouts.final_range_us + FinalRangeOverhead);
  }

  measurement_timing_budget_us = budget_us; // store for internal reuse
  return budget_us;
}

static int performSingleRefCalibration(uint8_t vhv_init_byte){
    int Timeout = 0;
    writeReg(REG_SYSRANGE_START, 0x01 | vhv_init_byte);
    while (((readReg(REG_RESULT_INTERRUPT_STATUS) & 0x07) == 0) && (Timeout < 100)){
        Timeout++; sleep_us(5000);
    }
    writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01);
    writeReg(REG_SYSRANGE_START, 0x00);
    return 1;
}

// --- Inicjalizacja Sensora ---
static int initSensor(int sensor_index, int boolLongRange){
    unsigned char spad_count=0, spad_type_is_aperture=0, ref_spad_map[6];
    unsigned char FirstSPAD, SPADsEnabled;
    printf("%d Inicjalizacja czujnika 0x%02X \n", sensor_index, i2cAddress);

    writeReg(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, readReg(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV) | 0x01);
    writeRegList(I2CMode);

    stop_variable[sensor_index] = readReg(0x91);
    printf("%d stop_variable = 0x%02X\n", sensor_index, stop_variable[sensor_index]);

    writeRegList(I2CMode2);
    writeReg(REG_MSRC_CONFIG_CONTROL, readReg(REG_MSRC_CONFIG_CONTROL) | 0x12);
    writeReg16(FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, 32); 
    writeReg(SYSTEM_SEQUENCE_CONFIG, 0xFF);
    getSpadInfo(&spad_count, &spad_type_is_aperture);
    readMulti(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

    writeRegList(SPAD_MAP_SETUP);
    
    FirstSPAD = (spad_type_is_aperture) ? 12 : 0;
    SPADsEnabled = 0;
    for (int i=0; i<48; i++){
        if (i < FirstSPAD || SPADsEnabled == spad_count) ref_spad_map[i>>3] &= ~(1<<(i & 7));
        else if (ref_spad_map[i>>3] & (1<< (i & 7))) SPADsEnabled++;
    }
    writeMulti(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);
    writeRegList(DefTuning);

    if (boolLongRange){
        writeReg16(FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, 13); // 0.1
        setVcselPulsePeriod(VcselPeriodPreRange, 18);
        setVcselPulsePeriod(VcselPeriodFinalRange, 14);
    }
    writeReg(SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);
    writeReg(GPIO_HV_MUX_ACTIVE_HIGH, readReg(GPIO_HV_MUX_ACTIVE_HIGH) & ~0x10);
    writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01);
    
    measurement_timing_budget_us = getMeasurementTimingBudget();
    writeReg(SYSTEM_SEQUENCE_CONFIG, 0xe8);    
    setMeasurementTimingBudget(measurement_timing_budget_us);
    
    writeReg(SYSTEM_SEQUENCE_CONFIG, 0x01);
    if (!performSingleRefCalibration(0x40)) { return 0; }
    writeReg(SYSTEM_SEQUENCE_CONFIG, 0x02);
    if (!performSingleRefCalibration(0x00)) { return 0; }
    writeReg(SYSTEM_SEQUENCE_CONFIG, 0xe8);

    printf("%d Inicjalizacja zakończona!\n", sensor_index);
    return 1;
}

// --- API Publiczne ---
int tofInit(int sensor_index, int Addr, int boolLongRange){
    i2cAddress = Addr;
    // current_sensor_index = sensor_index;
    return initSensor(sensor_index, boolLongRange);
}

void tofSetAddress(uint8_t oldAddr, uint8_t newAddr){
    uint8_t Temp[2] = {REG_I2C_SLAVE_DEVICE_ADDRESS, (uint8_t)(newAddr & 0x7F)};
    int result = i2c_write_blocking(i2c_default, oldAddr, Temp, 2, false);
    if(result!=2){
        printf("Blad zmiany adresu 0x%02X -> 0x%02X (zapisano=%d/2)\n", oldAddr, newAddr, result);
    }
    sleep_ms(20);
}

int tofReadDistance(int sensor_index){
    if(sensor_index <0 || sensor_index >=4){
        printf("Blad: zly indeks czujnika: %d\n", sensor_index);
        return -3;
    }
    int timeout = 0;
    writeReg(0x80, 0x01);
    writeReg(0xFF, 0x01);
    writeReg(0x00, 0x00);
    writeReg(0x91, stop_variable[sensor_index]);
    writeReg(0x00, 0x01);
    writeReg(0xFF, 0x00);
    writeReg(0x80, 0x00);

    writeReg(REG_SYSRANGE_START, 0x01);
    while ((readReg(REG_SYSRANGE_START) & 0x01) && timeout < 100){
        timeout++; sleep_us(5000);
    }
    if(timeout >= 100) {
        printf("Timeout START dla czujnika %d\n", sensor_index);
        return -1;
    }

    timeout = 0;
    while (((readReg(REG_RESULT_INTERRUPT_STATUS) & 0x07) == 0) && timeout < 100){
        timeout++; sleep_us(5000);
    }
    if(timeout >= 100){
        printf("Timeout START dla czujnika %d\n", sensor_index);
        return -2;
    } 

    int range = readReg16(RESULT_RANGE_STATUS + 10);
    writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01);
    return range;
}
void tofSetCurrentAddress(uint8_t addr){
    i2cAddress = addr;
}
// void tofSetCurrentSensor(int sensor_index){
//     if(sensor_index >=0 && sensor_index <4){
//         current_sensor_index = sensor_index;
//     }
// }

void IR_init(void){

    printf("Inicjjalizacja czujnikow\n");
    i2c_init(i2c_default, FREQ);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    printf("Zainicjalizowano I2C na pinach: SDA=%d, SCL=%d\n", I2C_SDA_PIN, I2C_SCL_PIN);

    int xshut[4] = {XSHUT4_PIN, XSHUT3_PIN, XSHUT2_PIN, XSHUT1_PIN};
    uint8_t addr[4] = {0x30, 0x31, 0x32, 0x33};

    printf("Wylaczenie czujnikow\n");
    for(int i=0; i<4; i++){
        gpio_init(xshut[i]);
        gpio_set_dir(xshut[i], GPIO_OUT);
        gpio_put(xshut[i], 0);
        printf("Czujnik %d, pin %d: OFF\n", i, xshut[i]);
    }
    sleep_ms(100);

    printf("Inicjalizacja czujnikow\n");
    for(int i=0; i<4; i++) {

        printf("Czujnik %d\n", i);
        gpio_put(xshut[i], 1);
        printf("Wlaczono XSHUT pin %d\n", xshut[i]);
        sleep_ms(50);
        
        printf("Adresowanie czujnika %d: 0x29 -> 0x%02X\n", i, addr[i]);
        tofSetAddress(0x29, addr[i]);
        sleep_ms(20);

        i2cAddress = addr[i];
        uint8_t id = readReg(0xC0);
        printf("ID pod 0x%02X: 0x%02X\n", addr[i], id);

        if(id != 0xEE && id != 0xFF){
            printf("Nieoczekiwane ID\n");
        }
        
        if (tofInit(i, addr[i], 1)) {
            printf("Czujnik %d OK (stop_variable = 0x%02X)\n", i, stop_variable[i]);
        } else {
            printf("Czujnik %d BLAD\n", i);
        }
        setMeasurementTimingBudget(100000);
        sleep_ms(50);
    }
    printf("Inicjalizacja zakonczona\n");
}

