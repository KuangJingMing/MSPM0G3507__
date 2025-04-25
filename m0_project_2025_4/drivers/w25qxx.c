#include "ti_msp_dl_config.h"
#include "delay.h"
#include "w25qxx.h"


#define spi_miss_up_wait_cnt 5000//500

#define W25Q64_CS_1   DL_GPIO_setPins(PORTB_PORT,   PORTB_W25Q64_CS_PIN)    //PB25
#define W25Q64_CS_0   DL_GPIO_clearPins(PORTB_PORT,   PORTB_W25Q64_CS_PIN)

uint16_t w25qxx_id = 0;
void W25QXX_PowerDown(void);
void W25QXX_WAKEUP(void);
void W25QXX_Erase_Chip(void);
void W25Q64_erase_sector(uint32_t addr);


void w25qxx_gpio_init(void)
{
  W25Q64_CS_1;
	//W25QXX_Erase_Chip();
	//W25Q64_erase_sector(0x00);

  W25QXX_PowerDown();
  delay_ms(10);
  W25QXX_WAKEUP();
  delay_ms(10);

  w25qxx_id = W25Q64_readID();	      //读取FLASH ID.
}


void w25qxx_erase_all(void)
{
  W25QXX_Erase_Chip();
  W25Q64_erase_sector(0x00);
}

#define fast_spi_enable 0



uint8_t spi_read_write_byte(uint8_t Byte)
{
#if fast_spi_enable==1
  SPI_0_INST->TXDATA = Byte;
  while((SPI_0_INST->STAT & SPI_STAT_BUSY_MASK) == SPI_STAT_BUSY_ACTIVE);
  return ((uint8_t)(SPI_0_INST->RXDATA));

//	DL_SPI_transmitData8(SPI_0_INST,Byte);
//	while (DL_SPI_isBusy(SPI_0_INST));
//	return  DL_SPI_receiveData8(SPI_0_INST);
#else
  DL_SPI_transmitData8(SPI_0_INST, Byte);  //发送1字节命令
	uint16_t tmp_try_miss_up_cnt=spi_miss_up_wait_cnt;
  while(DL_SPI_isBusy(SPI_0_INST)&&tmp_try_miss_up_cnt--){;}        //等待发送完成
  return DL_SPI_receiveData8(SPI_0_INST);  //清空接收FIFO中的数据
#endif
}






/******************************************************************
 * 函 数 名 称：W25Q64_readID
 * 函 数 说 明：读取W25Q64的厂商ID和设备ID
 * 函 数 形 参：无
 * 函 数 返 回：设备正常返回EF16
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
uint16_t W25Q64_readID(void)
{
  uint16_t  temp = 0;
  W25Q64_CS_0;

  spi_read_write_byte(0x90);//发送读取ID命令
  spi_read_write_byte(0x00);
  spi_read_write_byte(0x00);
  spi_read_write_byte(0x00);
  //接收数据
  temp |= spi_read_write_byte(0xFF) << 8;
  temp |= spi_read_write_byte(0xFF);

  W25Q64_CS_1;
  return temp;
}



/**********************************************************
 * 函 数 名 称：W25Q64_wait_busy
 * 函 数 功 能：判断W25Q64是否忙
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：无
**********************************************************/
void W25Q64_wait_busy(void)
{
  unsigned char byte = 0;

  do
  {
    W25Q64_CS_0;
    spi_read_write_byte(0x05);
    byte = spi_read_write_byte(0Xff);
    W25Q64_CS_1;
  }
  while( ( byte & 0x01 ) == 1 );
}


//进入掉电模式
void W25QXX_PowerDown(void)
{
  W25Q64_CS_0;                            //使能器件
  spi_read_write_byte(W25X_PowerDown);     //发送掉电命令
  W25Q64_CS_1;                            //取消片选
  delay_us(3);                            //等待TPD
}

//唤醒
void W25QXX_WAKEUP(void)
{
  W25Q64_CS_0;                                //使能器件
  spi_read_write_byte(W25X_ReleasePowerDown);  //send W25X_PowerDown command 0xAB
  W25Q64_CS_1;                                //取消片选
  delay_us(3);                                //等待TRES1
}



/**********************************************************
 * 函 数 名 称：W25Q64_write_enable
 * 函 数 功 能：发送写使能
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：无
**********************************************************/
void W25Q64_write_enable(void)
{
  W25Q64_CS_0;
  spi_read_write_byte(0x06);
  W25Q64_CS_1;
}

/**********************************************************
 * 函 数 名 称：W25Q64_erase_sector
 * 函 数 功 能：擦除一个扇区
 * 传 入 参 数：addr=擦除的扇区号
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：addr=擦除的扇区号，范围=0~15
**********************************************************/
void W25Q64_erase_sector(uint32_t addr)
{
  addr *= 4096;
  W25Q64_write_enable();  //写使能
  W25Q64_wait_busy();     //判断忙
  W25Q64_CS_0;
  spi_read_write_byte(0x20);
  spi_read_write_byte((uint8_t)((addr) >> 16));
  spi_read_write_byte((uint8_t)((addr) >> 8));
  spi_read_write_byte((uint8_t)addr);
  W25Q64_CS_1;
  //等待擦除完成
  W25Q64_wait_busy();
}


/**********************************************************
 * 函 数 名 称：W25Q64_write
 * 函 数 功 能：写数据到W25Q64进行保存
 * 传 入 参 数：buffer=写入的数据内容	addr=写入地址	numbyte=写入数据的长度
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：无
**********************************************************/
void W25Q64_write(uint8_t* buffer, uint32_t addr, uint16_t numbyte)
{
  //0x02e21
  unsigned int i = 0;
  W25Q64_erase_sector(addr / 4096); //擦除扇区数据
  W25Q64_write_enable();//写使能
  W25Q64_wait_busy(); //忙检测
  //写入数据
  W25Q64_CS_0;
  spi_read_write_byte(0x02);
  spi_read_write_byte((uint8_t)((addr) >> 16));
  spi_read_write_byte((uint8_t)((addr) >> 8));
  spi_read_write_byte((uint8_t)addr);

  for(i = 0; i < numbyte; i++)
  {
    spi_read_write_byte(buffer[i]);
  }

  W25Q64_CS_1;
  W25Q64_wait_busy(); //忙检测
}

/**********************************************************
 * 函 数 名 称：W25Q64_read
 * 函 数 功 能：读取W25Q64的数据
 * 传 入 参 数：buffer=读出数据的保存地址  read_addr=读取地址		read_length=读去长度
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：无
**********************************************************/
void W25Q64_read(uint8_t* buffer, uint32_t read_addr, uint16_t read_length)
{
  uint16_t i;
  W25Q64_CS_0;
  spi_read_write_byte(0x03);
  spi_read_write_byte((uint8_t)((read_addr) >> 16));
  spi_read_write_byte((uint8_t)((read_addr) >> 8));
  spi_read_write_byte((uint8_t)read_addr);

  for(i = 0; i < read_length; i++)
  {
    buffer[i] = spi_read_write_byte(0XFF);
  }

  W25Q64_CS_1;
}



//namelesstech
void W25QXX_Erase_Chip(void)
{
  W25Q64_write_enable();                  //SET WEL
  W25Q64_wait_busy();
  W25Q64_CS_0; //使能器件
  spi_read_write_byte(0xC7);     //发送片擦除命令
  W25Q64_CS_1; //取消片选
  W25Q64_wait_busy();   				          //等待芯片擦除结束
}


void W25Q64_write_page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint16_t i;
  W25Q64_write_enable();                  //SET WEL
  W25Q64_CS_0; //使能器件
  spi_read_write_byte(0x02);   //发送写页命令
  spi_read_write_byte((uint8_t)((WriteAddr) >> 16)); //发送24bit地址
  spi_read_write_byte((uint8_t)((WriteAddr) >> 8));
  spi_read_write_byte((uint8_t)WriteAddr);

  for(i = 0; i < NumByteToWrite; i++)	spi_read_write_byte(pBuffer[i]); //循环写数

  W25Q64_CS_1; //取消片选
  W25Q64_wait_busy();					   					//等待写入结束
}


//无检验写SPI FLASH
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void W25QXX_Write_NoCheck(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint16_t pageremain;
  pageremain = 256 - WriteAddr % 256; //单页剩余的字节数

  if(NumByteToWrite <= pageremain)pageremain = NumByteToWrite; //不大于256个字节

  while(1)
  {
    W25Q64_write_page(pBuffer, WriteAddr, pageremain);

    if(NumByteToWrite == pageremain)	break; //写入结束了
    else //NumByteToWrite>pageremain
    {
      pBuffer += pageremain;
      WriteAddr += pageremain;

      NumByteToWrite -= pageremain;			 //减去已经写入了的字节数

      if(NumByteToWrite > 256)pageremain = 256; //一次可以写入256个字节
      else pageremain = NumByteToWrite; 	 //不够256个字节了
    }
  }
}

//写SPI FLASH
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
uint8_t W25QXX_BUFFER[4096];
void W25QXX_Write(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint32_t secpos;
  uint16_t secoff;
  uint16_t secremain;
  uint16_t i;
  uint8_t * W25QXX_BUF;
  W25QXX_BUF = W25QXX_BUFFER;
  secpos = WriteAddr / 4096; //扇区地址
  secoff = WriteAddr % 4096; //在扇区内的偏移
  secremain = 4096 - secoff; //扇区剩余空间大小

  //printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
  if(NumByteToWrite <= secremain)secremain = NumByteToWrite; //不大于4096个字节

  while(1)
  {
    W25Q64_read(W25QXX_BUF, secpos * 4096, 4096); //读出整个扇区的内容

    for(i = 0; i < secremain; i++) //校验数据
    {
      if(W25QXX_BUF[secoff + i] != 0XFF)break; //需要擦除
    }

    if(i < secremain) //需要擦除
    {
      W25Q64_erase_sector(secpos); //擦除这个扇区

      for(i = 0; i < secremain; i++)	 //复制
      {
        W25QXX_BUF[i + secoff] = pBuffer[i];
      }

      W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096); //写入整个扇区

    }
    else W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain); //写已经擦除了的,直接写入扇区剩余区间.

    if(NumByteToWrite == secremain)break; //写入结束了
    else//写入未结束
    {
      secpos++;//扇区地址增1
      secoff = 0; //偏移位置为0

      pBuffer += secremain; //指针偏移
      WriteAddr += secremain; //写地址偏移
      NumByteToWrite -= secremain;				//字节数递减

      if(NumByteToWrite > 4096)secremain = 4096;	//下一个扇区还是写不完
      else secremain = NumByteToWrite;					//下一个扇区可以写完了
    }
  };
}


void W25QXX_Write_Data(float *data, uint32_t addr)
{
  uint8_t tempwr[4];
  tempwr[0] = (*(uint32_t *)(data)) & 0xff;
  tempwr[1] = (*(uint32_t *)(data)) >> 8;
  tempwr[2] = (*(uint32_t *)(data)) >> 16;
  tempwr[3] = (*(uint32_t *)(data)) >> 24;
  //W25QXX_Write_Page(tempwr,addr,4);
  W25QXX_Write(tempwr, addr, 4);
}


typedef union
{
  unsigned char Bdata[4];
  float Fdata;
  uint32_t Idata;
} Float_2_Byte;


void W25QXX_Read_Float(float *data, uint32_t addr)
{
  Float_2_Byte temp;
  W25Q64_read(temp.Bdata, addr, 4);
  *data = temp.Fdata;
}

void W25QXX_Read_f(float *pui32Data, uint32_t ui32Address, uint32_t ui32Count)
{
  Float_2_Byte temp;
  uint16_t i = 0;

  for(i = 0; i < ui32Count; i++)
  {
    W25Q64_read(temp.Bdata, ui32Address + 4 * i, 4);
    *pui32Data = temp.Fdata;
    pui32Data++;
  }
}


void W25QXX_Read_fs(float *Data, uint32_t ui32Address, uint32_t ui32Count)
{
  uint16_t i = 0;

  for(i = 0; i < ui32Count; i++)
  {
    Float_2_Byte temp;
    W25Q64_read(&temp.Bdata[0], ui32Address + 4 * i + 0, 1);
    W25Q64_read(&temp.Bdata[1], ui32Address + 4 * i + 1, 1);
    W25Q64_read(&temp.Bdata[2], ui32Address + 4 * i + 2, 1);
    W25Q64_read(&temp.Bdata[3], ui32Address + 4 * i + 3, 1);
    *Data = temp.Fdata;
    Data++;
  }
}


void W25QXX_Write_f(float *data, uint32_t ui32Address, uint32_t ui32Count)
{
  Float_2_Byte temp;
  uint16_t i = 0;

  for(i = 0; i < ui32Count; i++)
  {
    temp.Fdata = *data;
    W25QXX_Write(temp.Bdata, ui32Address + 4 * i, 4);
    data++;
  }
}






void Resume_Factory_Setting(void)
{
  W25QXX_Erase_Chip();
  W25Q64_erase_sector(0x00);
}

