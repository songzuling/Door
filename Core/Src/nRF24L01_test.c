
#include "nRF24L01_test.h"

extern SPI_HandleTypeDef hspi2;
static uint8_t testData = 0;

static void Delay(uint32_t ms);
static uint8_t EnterRxMode(void);
static uint8_t EnterTxMode(void);
static uint8_t EnterStandbyIMode(void);
static uint8_t EnterStandbyIIMode(void);
static uint8_t EnterPowerDownMode(void);
static uint8_t nRF24L01_Transmit1(uint8_t Cmd, uint8_t Reg, uint8_t* pData, uint32_t Size);
static uint8_t nRF24L01_Receive1(uint8_t Cmd, uint8_t Reg, uint8_t* pData, uint32_t Size, uint32_t Timeout);

uint8_t pdState1 = 0;

void nRF24L01_Init1(void)
{  
  CE1_L;
  CSN1_H;
  Delay(20); //上电10.3ms之后进入powerdown模式  
  pdState1 = EnterPowerDownMode();
  //  Delay(10);
  //  EnterStandbyIMode();
  //  Delay(10);
  EnterTxMode();
}


//72MHz while里面1个asm("nop");
// n = 10 大约 0.6us
// n = 5  大约 0.3us
// n = 2  大约 0.12us
#pragma optimize=none 
static void SPI_Delay(int n)
{
	while(n--)
	{
		asm("nop");
	}
}


int txCnt = 0;
void TestSend()
{  
  CE1_L;  
  txCnt++;
  static uint8_t tx_buf[10] ;  
  nRF24L01_Transmit1(nRF24L01_W_TX_PAYLOAD, 0, tx_buf, 5);     

  CE1_H;
  SPI_Delay(500);  
  tx_buf[4]++;
  return;
}

/*
 * 进入掉电模式
 */

static uint8_t EnterPowerDownMode(void)
{  
  uint8_t n = 10;
  uint8_t data = 0x08;  
  static uint8_t result = 0;      
  while(n--)
  {
    nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_CONFIG, &data, 1); 
    nRF24L01_Receive1(nRF24L01_R_REGISTER, nRF24L01_CONFIG, &result, 1, 20);
    if(result==data)
    {
        return 1;
    }    
  }
  return 0;
}


/*
 * 进入待机模式1
 */

static uint8_t EnterStandbyIMode(void)
{  
  CE1_L;
  uint8_t data = 0x00;        
  nRF24L01_Receive1(nRF24L01_R_REGISTER, nRF24L01_CONFIG, &data, 1, 10);
  data |= 0x02;
  nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_CONFIG, &data, 1); 
  CE1_H;
  if(0 == (data & 0x02))
  {
    return 0;
  }
  return 1;
}


/*
 * 进入待机模式2
 */
static uint8_t EnterStandbyIIMode(void)
{
  return 1;
}

/*
 * 进入接收模式
 */
static uint8_t EnterRxMode(void)
{

  uint8_t data = 0;
  //  该地址作为测试用而已 ///////////////////////
  uint8_t addr[5] = {0xB3, 0xB4, 0xB5, 0xB6, 0xF1};
  nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_RX_ADDR_P0, addr, 5);  /*写接收地址*/  
  data = 0x00;
  nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_EN_AA, &data, 1);      /*自动回复*/
  data = 0x01;
  nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_EN_RXADDR, &data, 1);  /*使能pipe0*/
  data = 40;
  nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_RF_CH, &data, 1);      /*配置通道*/
  data = 32;
  nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_RX_PW_P0, &data, 1);   /*配置接受数据的宽度*/
  data = 0x07;
  nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_RF_SETUP, &data, 1);   /**/
  data = 0x0F;
  nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_CONFIG, &data, 1);     /**/  
  CE1_L;
  return 1;
}


/*
 * 进入发射模式
 */
static uint8_t EnterTxMode(void)
{  
  uint8_t data = 0;
  static uint8_t testResult1[10];
  data = 0x00;
  nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_EN_RXADDR, &data, 1);   /*关闭pipe0*/  
  nRF24L01_Receive1(nRF24L01_R_REGISTER, nRF24L01_EN_RXADDR, testResult1, 1, 20);
  data = 0x01;
  nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_EN_RXADDR, &data, 1);   /*使能pipe0*/  
  nRF24L01_Receive1(nRF24L01_R_REGISTER, nRF24L01_EN_RXADDR, testResult1, 1, 20);
  data = 0x00;
  nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_EN_AA, &data, 1);       /*关闭自动回复*/  
  nRF24L01_Receive1(nRF24L01_R_REGISTER, nRF24L01_EN_AA, testResult1, 1, 20);
  data = 10;
  nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_RF_CH, &data, 1);       /*配置通道*/  
  nRF24L01_Receive1(nRF24L01_R_REGISTER, nRF24L01_RF_CH, testResult1, 1, 20);
  data = 0x0F;
  nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_RF_SETUP, &data, 1);    /**/  
  nRF24L01_Receive1(nRF24L01_R_REGISTER, nRF24L01_RF_SETUP, testResult1, 1, 20);
  data = 0x7A;
  nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_CONFIG, &data, 1);      /**/
  nRF24L01_Receive1(nRF24L01_R_REGISTER, nRF24L01_CONFIG, testResult1, 1, 20);
  
  uint8_t addr[5] = {0xB3, 0xB4, 0xB5, 0xB6, 0xF1}; 
  nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_TX_ADDR, addr, 5);    /*写发送地址*/
  nRF24L01_Receive1(nRF24L01_R_REGISTER, nRF24L01_TX_ADDR, testResult1, 5, 20);
  nRF24L01_Transmit1(nRF24L01_W_REGISTER, nRF24L01_RX_ADDR_P0, addr, 5); /*写接收地址*/
  nRF24L01_Receive1(nRF24L01_R_REGISTER, nRF24L01_RX_ADDR_P0, testResult1, 5, 20);
  
  data = 0;
  nRF24L01_Transmit1(nRF24L01_FLUSH_TX, 0, &data, 0);      /**/
  
  CE1_H;
  SPI_Delay(200);
  return 1;
}

/*
 *
 *
 *
 */
static uint8_t nRF24L01_Transmit1(uint8_t Cmd, uint8_t Reg, uint8_t* pData, uint32_t Size)
{  
  uint8_t dat = Cmd + Reg;
  CSN1_H;
  CSN1_L;
  if(HAL_OK != HAL_SPI_Transmit(&hspi2, &dat, 1, 10))
  {
    return 0;
  }
  if(Size > 0)
  {
    if(HAL_OK != HAL_SPI_Transmit(&hspi2, pData, Size, 10))
    {
      return 0;
    }
  }
  CSN1_H;
  return 1;  
}


static uint8_t nRF24L01_Receive1(uint8_t Cmd, uint8_t Reg, uint8_t* pData, uint32_t Size, uint32_t Timeout)
{
  CSN1_H;
  CSN1_L;
  uint8_t dat = Cmd + Reg;
  if(HAL_OK != HAL_SPI_Transmit(&hspi2, &dat, 1, 10))
  {
    return 0;
  }
  if(Size > 0)
  {
    if(HAL_OK != HAL_SPI_Receive(&hspi2, pData, Size, Timeout) )
    {
      return 0;
    }
  }
  CSN1_H;
  return 1;
}


static void Delay(uint32_t ms)
{
  uint32_t tickstart = HAL_GetTick();
  
  while( HAL_GetTick() - tickstart < ms ){};
  
  return;
}