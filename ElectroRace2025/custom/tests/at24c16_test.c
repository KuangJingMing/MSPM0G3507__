#include "common_include.h"
#include "tests.h"
#include "log_config.h"
#include "log.h"
#include <math.h> // For fabs

#define ARRAY_SIZE 6
#define FLOAT_EPSILON 1e-6

float eeprom_write[ARRAY_SIZE] = {2, 2, 3, 5, 6, 2};
float eeprom_read[ARRAY_SIZE] = {0};

void at24cxx_single_test(void)
{
    log_i("EEPROM unit test start");

    AT24CXX_Init();
    log_i("AT24CXX initialization done");

    // Data write
    WriteFlashParameter(0, eeprom_write[0]);
    WriteFlashParameter_Two(1, eeprom_write[1], eeprom_write[2]);
    WriteFlashParameter_Three(3, eeprom_write[3], eeprom_write[4], eeprom_write[5]);
    
    // Data read
    ReadFlashParameterThree(0, &eeprom_read[0], &eeprom_read[1], &eeprom_read[2]);
    ReadFlashParameterTwo(3, &eeprom_read[3], &eeprom_read[4]);
    ReadFlashParameterOne(5, &eeprom_read[5]);

    // Compare results
    int error_count = 0;
    for(int i = 0; i < ARRAY_SIZE; ++i) {
        if (fabs(eeprom_write[i] - eeprom_read[i]) < FLOAT_EPSILON) {
            log_i("Compare OK: eeprom_write[%d]=%f, eeprom_read[%d]=%f", i, eeprom_write[i], i, eeprom_read[i]);
        } else {
            log_e("Compare FAIL: eeprom_write[%d]=%f, eeprom_read[%d]=%f", i, eeprom_write[i], i, eeprom_read[i]);
            error_count++;
        }
    }

    if(error_count == 0) {
        log_i("EEPROM read/write comparison passed for all items");
    } else {
        log_e("EEPROM read/write comparison failed: %d mismatches", error_count);
    }

    log_i("EEPROM unit test complete, enter endless loop and wait.");
    while (1) {
        // Endless loop
    }
}
