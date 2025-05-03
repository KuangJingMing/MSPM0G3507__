#include "eeprom_parameter.h"
#include "delay.h"

FLIGHT_PARAMETER Flight_Params =
{
  .num = FLIGHT_PARAMETER_TABLE_NUM
};


typedef union
{
  unsigned char Bdata[4];
  float Fdata;
  uint32_t Idata;
} FBI;

static FBI data;


void EEPROMRead(uint32_t *pui32Data, uint32_t ui32Address, uint32_t ui32Count)
{
  for(uint16_t i = 0; i < ui32Count; i++)
  {
    AT24CXX_Read(ui32Address + 4 * i, data.Bdata, 4);
    *pui32Data = data.Idata;
    pui32Data++;
  }
}


uint32_t EEPROMProgram(uint32_t *pui32Data, uint32_t ui32Address, uint32_t ui32Count)
{
  for(uint16_t i = 0; i < ui32Count; i++)
  {
    data.Idata = *pui32Data;
    AT24CXX_Write(ui32Address + 4 * i, data.Bdata, 4);
    pui32Data++;
  }

  return 1;
}


union
{
  unsigned char floatByte[4];
  float floatValue;
} _FloatUnion;

/***************************************************************************************
@函数名：void Float2Byte(float *FloatValue, unsigned char *Byte, unsigned char Subscript)
@入口参数：FloatValue:float值
			     Byte:数组
		       Subscript:指定从数组第几个元素开始写入
@出口参数：无
功能描述：将float数据转成4字节数据并存入指定地址
@作者：无名小哥
@日期：2020年01月17日
****************************************************************************************/
void _Float2Byte(float *FloatValue, volatile unsigned char *Byte, unsigned char Subscript)
{
  _FloatUnion.floatValue = (float)2;

  if(_FloatUnion.floatByte[0] == 0)//小端模式
  {
    _FloatUnion.floatValue = *FloatValue;
    Byte[Subscript]     = _FloatUnion.floatByte[0];
    Byte[Subscript + 1] = _FloatUnion.floatByte[1];
    Byte[Subscript + 2] = _FloatUnion.floatByte[2];
    Byte[Subscript + 3] = _FloatUnion.floatByte[3];
  }
  else//大端模式
  {
    _FloatUnion.floatValue = *FloatValue;
    Byte[Subscript]     = _FloatUnion.floatByte[3];
    Byte[Subscript + 1] = _FloatUnion.floatByte[2];
    Byte[Subscript + 2] = _FloatUnion.floatByte[1];
    Byte[Subscript + 3] = _FloatUnion.floatByte[0];
  }
}


/***************************************************************************************
@函数名：void Byte2Float(unsigned char *Byte, unsigned char Subscript, float *FloatValue)
@入口参数：Byte:数组
			     Subscript:指定从数组第几个元素开始写入
		       FloatValue:float值
@出口参数：无
功能描述：从指定地址将4字节数据转成float数据
@作者：无名小哥
@日期：2020年01月17日
****************************************************************************************/
void _Byte2Float(volatile unsigned char *Byte, unsigned char Subscript, float *FloatValue)
{
  _FloatUnion.floatByte[0] = Byte[Subscript];
  _FloatUnion.floatByte[1] = Byte[Subscript + 1];
  _FloatUnion.floatByte[2] = Byte[Subscript + 2];
  _FloatUnion.floatByte[3] = Byte[Subscript + 3];
  *FloatValue = _FloatUnion.floatValue;
}


volatile unsigned char _byte[4];

void EEPROMRead_f(float *Data, uint32_t ui32Address, uint32_t ui32Count)
{
  for(uint16_t i = 0; i < ui32Count; i++)
  {
    //AT24CXX_Read(ui32Address+4*i,(unsigned char *)_byte,4);
    _byte[0] = AT24CXX_ReadOneByte(ui32Address + 4 * i + 0);
    _byte[1] = AT24CXX_ReadOneByte(ui32Address + 4 * i + 1);
    _byte[2] = AT24CXX_ReadOneByte(ui32Address + 4 * i + 2);
    _byte[3] = AT24CXX_ReadOneByte(ui32Address + 4 * i + 3);
    _Byte2Float(_byte, 0, Data);
    Data++;
  }
}

uint32_t EEPROMProgram_f(float *Data, uint32_t ui32Address, uint32_t ui32Count)
{
  for(uint16_t i = 0; i < ui32Count; i++)
  {
    _Float2Byte(Data, _byte, 0);
    //AT24CXX_Write(ui32Address+4*i,(unsigned char *)_byte,4);
    AT24CXX_WriteOneByte(ui32Address + 4 * i + 0, _byte[0]);
    delay_ms(50);
    AT24CXX_WriteOneByte(ui32Address + 4 * i + 1, _byte[1]);
    delay_ms(50);
    AT24CXX_WriteOneByte(ui32Address + 4 * i + 2, _byte[2]);
    delay_ms(50);
    AT24CXX_WriteOneByte(ui32Address + 4 * i + 3, _byte[3]);
    delay_ms(50);
    Data++;
  }
  return 1;
}







#define WP_FLASH_BASE 0
static float eeprom_write_data[3] = {0, 0, 0};
void ReadFlashParameterALL(FLIGHT_PARAMETER *WriteData)
{
  #if eeprom_mode==0
	EEPROMRead((uint32_t *)(&WriteData->parameter_table), WP_FLASH_BASE, FLIGHT_PARAMETER_TABLE_NUM);
  #elif eeprom_mode==1
  EEPROMRead_f((float *)(&WriteData->parameter_table), WP_FLASH_BASE, FLIGHT_PARAMETER_TABLE_NUM);
	#else
	W25QXX_Read_f((float *)(&WriteData->parameter_table), WP_FLASH_BASE, FLIGHT_PARAMETER_TABLE_NUM);
  #endif
}

void ReadFlashParameterOne(uint16_t Label, float *ReadData)
{
  #if eeprom_mode==0
  EEPROMRead((uint32_t *)(ReadData), WP_FLASH_BASE + 4 * Label, 1);
  #elif eeprom_mode==1
  EEPROMRead_f((float *)(ReadData), WP_FLASH_BASE + 4 * Label, 1);
	#else
	W25QXX_Read_f((float *)(ReadData), WP_FLASH_BASE + 4 * Label, 1);
  #endif
}

void ReadFlashParameterTwo(uint16_t Label, float *ReadData1, float *ReadData2)
{
  #if eeprom_mode==0
  EEPROMRead((uint32_t *)(ReadData1), WP_FLASH_BASE + 4 * Label, 1);;
  EEPROMRead((uint32_t *)(ReadData2), WP_FLASH_BASE + 4 * Label + 4, 1);
  #elif eeprom_mode==1
  EEPROMRead_f((float *)(ReadData1), WP_FLASH_BASE + 4 * Label, 1);;
  EEPROMRead_f((float *)(ReadData2), WP_FLASH_BASE + 4 * Label + 4, 1);
	#else
  W25QXX_Read_f((float *)(ReadData1), WP_FLASH_BASE + 4 * Label, 1);;
  W25QXX_Read_f((float *)(ReadData2), WP_FLASH_BASE + 4 * Label + 4, 1);	
  #endif
}

void ReadFlashParameterThree(uint16_t Label, float *ReadData1, float *ReadData2, float *ReadData3)
{
  #if eeprom_mode==0
  EEPROMRead((uint32_t *)(ReadData1), WP_FLASH_BASE + 4 * Label, 1);;
  EEPROMRead((uint32_t *)(ReadData2), WP_FLASH_BASE + 4 * Label + 4, 1);
  EEPROMRead((uint32_t *)(ReadData3), WP_FLASH_BASE + 4 * Label + 8, 1);
  #elif eeprom_mode==1
  EEPROMRead_f((float *)(ReadData1), WP_FLASH_BASE + 4 * Label, 1);;
  EEPROMRead_f((float *)(ReadData2), WP_FLASH_BASE + 4 * Label + 4, 1);
  EEPROMRead_f((float *)(ReadData3), WP_FLASH_BASE + 4 * Label + 8, 1);
  #else
  W25QXX_Read_f((float *)(ReadData1), WP_FLASH_BASE + 4 * Label, 1);;
  W25QXX_Read_f((float *)(ReadData2), WP_FLASH_BASE + 4 * Label + 4, 1);
  W25QXX_Read_f((float *)(ReadData3), WP_FLASH_BASE + 4 * Label + 8, 1);
  #endif
}

void WriteFlashParameter(uint16_t Label,
                         float WriteData)
{
  eeprom_write_data[0] = WriteData; //将需要更改的字段赋新
  #if eeprom_mode==0
  EEPROMProgram((uint32_t *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 1);
  #elif eeprom_mode==1
  EEPROMProgram_f((float *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 1);
	#else
	W25QXX_Write_f((float *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 1);
  #endif
}



void WriteFlashParameter_Two(uint16_t Label,
                             float WriteData1,
                             float WriteData2)
{
  eeprom_write_data[0] = WriteData1; //将需要更改的字段赋新=WriteData1;//将需要更改的字段赋新值
  eeprom_write_data[1] = WriteData2; //将需要更改的字段赋新=WriteData2;//将需要更改的字段赋新值
  #if eeprom_mode==0
  EEPROMProgram((uint32_t *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 2);
  #elif eeprom_mode==1
  EEPROMProgram_f((float *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 2);
	#else
	W25QXX_Write_f((float *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 2);
  #endif
}

void WriteFlashParameter_Three(uint16_t Label,
                               float WriteData1,
                               float WriteData2,
                               float WriteData3)
{
  eeprom_write_data[0] = WriteData1; //将需要更改的字段赋新值
  eeprom_write_data[1] = WriteData2; //将需要更改的字段赋新值
  eeprom_write_data[2] = WriteData3; //将需要更改的字段赋新值
  #if eeprom_mode==0
  EEPROMProgram((uint32_t *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 3);
  #elif eeprom_mode==1
  EEPROMProgram_f((float *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 3);
	#else
	W25QXX_Write_f((float *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 3);
  #endif
}


void flight_read_flash_full(void)
{
  ReadFlashParameterALL((FLIGHT_PARAMETER *)(&Flight_Params));

  for(uint16_t i = 0; i < Flight_Params.num; i++)
  {
    if(isnan(Flight_Params.parameter_table[i]) == 0)
      Flight_Params.health[i] = true;
  }
}