
#include "nRF24L01.h"

extern SPI_HandleTypeDef hspi1;

void nRF24L01_Init(void)
{
  Delay(20);
  nRF24L01_EnterPowerDownMode();
  nRF24L01_EnterRxMode();  
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

/*
 * 进入掉电模式
 */
uint8_t nRF24L01_EnterPowerDownMode(void)
{ 
  uint8_t n = 10;
  uint8_t data = 0x08;  
  uint8_t result = 0;      
  while(n--)
  {
    nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_CONFIG, &data, 1); 
    nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_CONFIG, &result, 1, 20);
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

uint8_t nRF24L01_EnterStandbyIMode(void)
{  
  uint8_t data = 0x0A;        
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_CONFIG, &data, 1);   
  return 1;
}


/*
 * 进入待机模式2
 */
uint8_t nRF24L01_EnterStandbyIIMode(void)
{
  return 1;
}

/*
 * 进入接收模式
 */
uint8_t nRF24L01_EnterRxMode(void)
{  
  CE_L;
  static uint8_t testResult[10];
  uint8_t data = 0; 
  data = 0x7B;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_CONFIG, &data, 1);     /**/  
  nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_CONFIG, testResult, 1, 20);
  data = 0x00;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_EN_RXADDR, &data, 1);  /*关闭通道*/
  nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_EN_RXADDR, testResult, 1, 20);
  data = 0x01;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_EN_RXADDR, &data, 1);  /*使能pipe0*/
  nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_EN_RXADDR, testResult, 1, 20);
  data = 0x00;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_EN_AA, &data, 1);      /*关闭自动回复*/
  nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_EN_AA, testResult, 1, 20);
  data = 5;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_RX_PW_P0, &data, 1);   /*配置接受数据的宽度*/
  nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_RX_PW_P0, testResult, 1, 20);
  uint8_t addr[5] = {0xB3, 0xB4, 0xB5, 0xB6, 0xF1};
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_RX_ADDR_P0, addr, 5);  /*写接收地址*/  
  nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_RX_ADDR_P0, testResult, 5, 20);
  data = 10;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_RF_CH, &data, 1);      /*配置通道*/
  nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_RF_CH, testResult, 1, 20);
  data = 0x0F;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_RF_SETUP, &data, 1);   /**/
  nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_RF_SETUP, testResult, 1, 20);
  CE_H;
  SPI_Delay(300); /*~180us*/
  
  return 1;
  
}

/*
 * 进入发射模式
 */
uint8_t nRF24L01_EnterTxMode(void)
{
  
  CE_L;
  uint8_t data = 0;
  //  该地址作为测试用而已 ///////////////////////
  uint8_t addr[5] = {0xB3, 0xB4, 0xB5, 0xB6, 0xF1};
  uint8_t tx_buf[5] = {123, 456, 0x02, 0x03, 0x09};
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_TX_ADDR, addr, 5);    /*写发送地址*/
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_RX_ADDR_P0, addr, 5); /*写接收地址*/
  nRF24L01_Transmit(nRF24L01_W_TX_PAYLOAD, 0, tx_buf, 5); 
  
  data = 0x01;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_EN_AA, &data, 1);       /*自动回复*/
  
  data = 0x01;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_EN_RXADDR, &data, 1);   /*使能pipe0*/
  
  data = 0x1A;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_SETUP_RETR, &data, 1);  /**/
  
  data = 40;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_RF_CH, &data, 1);       /*配置通道*/
  
  data = 0x07;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_RF_SETUP, &data, 1);    /**/
  
  data = 0x0E;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_CONFIG, &data, 1);      /**/

  CE_H;
  return 1;
}

void nRF24L01_FlushTxFIFO(void)
{
  uint8_t data = nRF24L01_FLUSH_TX;
  HAL_SPI_Transmit(&hspi1, &data, 1, 10);   
  return ;
}

void nRF24L01_FlushRxFIFO(void)
{
  uint8_t data = nRF24L01_FLUSH_RX;
  HAL_SPI_Transmit(&hspi1, &data, 1, 10);   
  return ;
}

void nRF24L01_ResetTxDs(void)
{
  uint8_t data = 0x2E;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_STATUS, &data, 1);     
  return ;
}


void nRF24L01_ResetRxDs(void)
{
  uint8_t data = 0x4E;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_STATUS, &data, 1);     
  return ;
}
/*
 *
 *
 *
 */
uint8_t nRF24L01_Transmit(uint8_t Cmd, uint8_t Reg, uint8_t* pData, uint32_t Size)
{  
  uint8_t dat = Cmd + Reg;
  CSN_H;
  CSN_L;
  if(HAL_OK != HAL_SPI_Transmit(&hspi1, &dat, 1, 10))
  {
    return 0;
  }
  if(HAL_OK != HAL_SPI_Transmit(&hspi1, pData, Size, 10))
  {
    return 0;
  }
  CSN_H;
  return 1;  
}


uint8_t nRF24L01_Receive(uint8_t Cmd, uint8_t Reg, uint8_t* pData, uint32_t Size, uint32_t Timeout)
{
  CSN_H;
  CSN_L;
  uint8_t dat = Cmd + Reg;
  if(HAL_OK != HAL_SPI_Transmit(&hspi1, &dat, 1, 10))
  {
    return 0;
  }
  if(HAL_OK != HAL_SPI_Receive(&hspi1, pData, Size, Timeout) )
  {
    return 0;
  }
  CSN_H;
  return 1;
}


void Delay(uint32_t ms)
{
  uint32_t tickstart = HAL_GetTick();
  
  while( HAL_GetTick() - tickstart < ms ){};
  
  return;
}



int testRxCnt = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  testRxCnt++;
}

