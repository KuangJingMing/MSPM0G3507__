#include "ti_msp_dl_config.h"
#include "eeprom.h"
#include "w25qxx.h"


FLIGHT_PARAMETER Trackless_Params;
#define WP_FLASH_BASE 0
static float eeprom_write_data[3] = {0, 0, 0};
void ReadFlashParameterALL(FLIGHT_PARAMETER *WriteData)
{
	W25QXX_Read_f((float *)(&WriteData->parameter_table), WP_FLASH_BASE, FLIGHT_PARAMETER_TABLE_NUM);
}

void ReadFlashParameterOne(uint16_t Label, float *ReadData)
{
	W25QXX_Read_f((float *)(ReadData), WP_FLASH_BASE + 4 * Label, 1);
}

void ReadFlashParameterTwo(uint16_t Label, float *ReadData1, float *ReadData2)
{
  W25QXX_Read_f((float *)(ReadData1), WP_FLASH_BASE + 4 * Label, 1);;
  W25QXX_Read_f((float *)(ReadData2), WP_FLASH_BASE + 4 * Label + 4, 1);	
}

void ReadFlashParameterThree(uint16_t Label, float *ReadData1, float *ReadData2, float *ReadData3)
{
  W25QXX_Read_f((float *)(ReadData1), WP_FLASH_BASE + 4 * Label, 1);;
  W25QXX_Read_f((float *)(ReadData2), WP_FLASH_BASE + 4 * Label + 4, 1);
  W25QXX_Read_f((float *)(ReadData3), WP_FLASH_BASE + 4 * Label + 8, 1);
}

void WriteFlashParameter(uint16_t Label,
                         float WriteData,FLIGHT_PARAMETER *Table)
{
	Table->Parameter_Table[Label]=WriteData;//将需要更改的字段赋新
  eeprom_write_data[0] = WriteData; //将需要更改的字段赋新
	W25QXX_Write_f((float *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 1);
}



void WriteFlashParameter_Two(uint16_t Label,
                             float WriteData1,
                             float WriteData2,FLIGHT_PARAMETER *Table)
{
  Table->Parameter_Table[Label]=WriteData1;//将需要更改的字段赋新值
  Table->Parameter_Table[Label+1]=WriteData2;//将需要更改的字段赋新值
  eeprom_write_data[0] = WriteData1; //将需要更改的字段赋新=WriteData1;//将需要更改的字段赋新值
  eeprom_write_data[1] = WriteData2; //将需要更改的字段赋新=WriteData2;//将需要更改的字段赋新值
	W25QXX_Write_f((float *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 2);
}

void WriteFlashParameter_Three(uint16_t Label,
                               float WriteData1,
                               float WriteData2,
                               float WriteData3,FLIGHT_PARAMETER *Table)
{
  Table->Parameter_Table[Label]=WriteData1;//将需要更改的字段赋新值
  Table->Parameter_Table[Label+1]=WriteData2;//将需要更改的字段赋新值
  Table->Parameter_Table[Label+2]=WriteData3;//将需要更改的字段赋新值
  eeprom_write_data[0] = WriteData1; //将需要更改的字段赋新值
  eeprom_write_data[1] = WriteData2; //将需要更改的字段赋新值
  eeprom_write_data[2] = WriteData3; //将需要更改的字段赋新值
	W25QXX_Write_f((float *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 3);
}



FLIGHT_PARAMETER Flight_Params =
{
  .num = FLIGHT_PARAMETER_TABLE_NUM
};

void flight_read_flash_full(void)
{
  ReadFlashParameterALL((FLIGHT_PARAMETER *)(&Flight_Params));

  for(uint16_t i = 0; i < Flight_Params.num; i++)
  {
    if(isnan(Flight_Params.parameter_table[i]) == 0)
      Flight_Params.health[i] = true;
  }
}
