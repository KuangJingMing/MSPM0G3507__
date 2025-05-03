#ifndef EEPROM_PARAMETER_H__
#define EEPROM_PARAMETER_H__

#include "driver_at24cxx.h"

#define eeprom_mode 1

#define PARAMETER_TABLE_STARTADDR   0x0000 
#define FLIGHT_PARAMETER_TABLE_NUM  200

typedef struct
{
	float parameter_table[FLIGHT_PARAMETER_TABLE_NUM];
	bool health[FLIGHT_PARAMETER_TABLE_NUM];
	uint32_t num;
}FLIGHT_PARAMETER;

typedef enum
{
	GYRO_X_OFFSET = 0,
	GYRO_Y_OFFSET,
	GYRO_Z_OFFSET,
	ACCEL_X_OFFSET,
	ACCEL_Y_OFFSET,
	ACCEL_Z_OFFSET
}FLIGHT_PARAMETER_TABLE;

extern FLIGHT_PARAMETER Flight_Params;		

void flight_read_flash_full(void);

void EEPROMRead(uint32_t *pui32Data, uint32_t ui32Address, uint32_t ui32Count);
uint32_t EEPROMProgram(uint32_t *pui32Data, uint32_t ui32Address, uint32_t ui32Count);
uint32_t EEPROMProgram_f(float *Data, uint32_t ui32Address, uint32_t ui32Count);
void EEPROMRead_f(float *Data, uint32_t ui32Address, uint32_t ui32Count);

void ReadFlashParameterALL(FLIGHT_PARAMETER *WriteData);
void ReadFlashParameterOne(uint16_t Label,float *ReadData);
void ReadFlashParameterTwo(uint16_t Label,float *ReadData1,float *ReadData2);
void ReadFlashParameterThree(uint16_t Label,float *ReadData1,float *ReadData2,float *ReadData3);
   
   
void WriteFlashParameter(uint16_t Label,float WriteData);
void WriteFlashParameter_Two(uint16_t Label,
                         float WriteData1,
                         float WriteData2);
void WriteFlashParameter_Three(uint16_t Label,
                         float WriteData1,
                         float WriteData2,
                         float WriteData3);
void ReadFlashParameterTwo(uint16_t Label,float *ReadData1,float *ReadData2);

#endif 
