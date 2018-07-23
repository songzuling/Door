
#include "nRF24L01.h"

extern SPI_HandleTypeDef hspi1;

uint8_t pdState = 0;
void nRF24L01_Init(void)
{
  CE_L;
  CSN_H;
  Delay(15);
  nRF24L01_ResetRxDs();
  pdState = nRF24L01_EnterPowerDownMode();
  nRF24L01_EnterRxMode();  
}

//72MHz while����1��asm("nop");
// n = 10 ��Լ 0.6us
// n = 5  ��Լ 0.3us
// n = 2  ��Լ 0.12us
#pragma optimize=none 
static void SPI_Delay(int n)
{
	while(n--)
	{
		asm("nop");
	}
}

/*
* �������ģʽ
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
* �������ģʽ1
*/

uint8_t nRF24L01_EnterStandbyIMode(void)
{  
  uint8_t data = 0x0A;        
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_CONFIG, &data, 1);   
  return 1;
}


/*
* �������ģʽ2
*/
uint8_t nRF24L01_EnterStandbyIIMode(void)
{
  return 1;
}

/*
* �������ģʽ
*/
uint8_t nRF24L01_EnterRxMode(void)
{  
  CE_L;
  static uint8_t testResult[10];
  uint8_t data = 0;  
  data = 0x00;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_EN_RXADDR, &data, 1);  /*�ر�ͨ��*/
  nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_EN_RXADDR, testResult, 1, 20);
  data = 0x01;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_EN_RXADDR, &data, 1);  /*ʹ��pipe0*/
  nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_EN_RXADDR, testResult, 1, 20);
  data = 0x00;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_EN_AA, &data, 1);      /*�Զ��ظ�*/
  nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_EN_AA, testResult, 1, 20);
  data = 5;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_RX_PW_P0, &data, 1);   /*���ý������ݵĿ��*/
  nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_RX_PW_P0, testResult, 1, 20);
  uint8_t addr[5] = {0xB3, 0xB4, 0xB5, 0xB6, 0xF1};
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_RX_ADDR_P0, addr, 5);  /*д���յ�ַ*/  
  nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_RX_ADDR_P0, testResult, 5, 20);
  data = 10;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_RF_CH, &data, 1);      /*����ͨ��*/
  nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_RF_CH, testResult, 1, 20);
  data = 0x0F;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_RF_SETUP, &data, 1);   /**/
  nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_RF_SETUP, testResult, 1, 20);
  data = 0;
  nRF24L01_Transmit(nRF24L01_FLUSH_RX, 0, &data, 0);      /**/
  data = 0x0B;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_CONFIG, &data, 1);     /**/  
  nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_CONFIG, testResult, 1, 20);  
  CE_H;
  SPI_Delay(300); /*~180us*/
  
  return 1;
  
}

/*
* ���뷢��ģʽ
*/
uint8_t nRF24L01_EnterTxMode(void)
{
  
  CE_L;
  uint8_t data = 0;
  //  �õ�ַ��Ϊ�����ö��� ///////////////////////
  uint8_t addr[5] = {0xB3, 0xB4, 0xB5, 0xB6, 0xF1};
  uint8_t tx_buf[5] = {123, 456, 0x02, 0x03, 0x09};
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_TX_ADDR, addr, 5);    /*д���͵�ַ*/
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_RX_ADDR_P0, addr, 5); /*д���յ�ַ*/
  nRF24L01_Transmit(nRF24L01_W_TX_PAYLOAD, 0, tx_buf, 5); 
  
  data = 0x01;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_EN_AA, &data, 1);       /*�Զ��ظ�*/
  
  data = 0x01;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_EN_RXADDR, &data, 1);   /*ʹ��pipe0*/
  
  data = 0x1A;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_SETUP_RETR, &data, 1);  /**/
  
  data = 40;
  nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_RF_CH, &data, 1);       /*����ͨ��*/
  
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

uint8_t testDs = 0;
uint8_t testDsState = 0;
void nRF24L01_ResetRxDs(void)
{
  uint8_t data = 0;
  testDsState = nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_STATUS, &data, 1, 10); 
  testDs = data;  
  data |= 0x7E;
  testDs = data;
  testDsState = nRF24L01_Transmit(nRF24L01_W_REGISTER, nRF24L01_STATUS, &data, 1);     
  testDsState = nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_STATUS, &data, 1, 10); 
  testDs = data;  
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
  if(Size > 0)
  {
    if(HAL_OK != HAL_SPI_Transmit(&hspi1, pData, Size, 10))
    {
      return 0;
    }
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
  if(Size > 0)
  {
    if(HAL_OK != HAL_SPI_Receive(&hspi1, pData, Size, Timeout) )
    {
      return 0;
    }
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

uint8_t rxData[5];
int irqCnt = 0;
int testRxCnt = 0;
uint8_t isGetNewData = 0;
uint8_t isIrq = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{  
  isIrq = 1;
  irqCnt++;
}

int testRxCnt1 = 0;
int testRxCnt2 = 0;

void WaiteForData(void)
{

  uint32_t tick = HAL_GetTick();
  while(0==isGetNewData)
  {
    if(isIrq)
    {
      isIrq = 0;
      Delay(1);
      testDsState = nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_STATUS, &testDs, 1, 10); 
      if(testDs&0x40)//> 0)
      {
        nRF24L01_Receive(nRF24L01_R_RX_PAYLOAD, 0, rxData, 5, 50);               
        testRxCnt++; 
        isGetNewData = 1;        
      }  
      if(testDs&0x20)
      {
        testRxCnt1++;
        isGetNewData = 1;     
      }
      if(testDs&0x10)
      {
        testRxCnt2++;
        isGetNewData = 1;     
      }
//      else
//      {
        uint8_t data = 0;
//        nRF24L01_Transmit(nRF24L01_FLUSH_RX, 0, &data, 0);      /**/
//      }
//      nRF24L01_Receive(nRF24L01_R_RX_PAYLOAD, 0, rxData, 5, 50);           
      nRF24L01_Transmit(nRF24L01_FLUSH_RX, 0, &data, 0);      /**/
      nRF24L01_ResetRxDs();
    }
    if(HAL_GetTick() - tick > 100 )
    {
      nRF24L01_ResetRxDs();
      break;
    }
  };
  isGetNewData = 0;  
  
  
//  static uint8_t rxData[5];
//  testDs = 0;
//  testDsState = nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_STATUS, &testDs, 1, 10); 
//  nRF24L01_Receive(nRF24L01_R_RX_PAYLOAD, 0, rxData, 5, 50);       
//  testDsState = nRF24L01_Receive(nRF24L01_R_REGISTER, nRF24L01_STATUS, &testDs, 1, 10); 
////  if(testDs)
////  {
////    testRxCnt++; 
////    isGetNewData = 1;        
////  }  
//  nRF24L01_ResetRxDs();
}




