
#ifndef __NRF24L01_TEST_H__
#define __NRF24L01_TEST_H__


#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "string.h"

/*chip enable*/
#define CE_L1  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
#define CE_H1  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

/*spi chip select*/
#define CSN_L1  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
#define CSN_H1  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);


//nRF24L01 Register Definitions
#define nRF24L01_CONFIG       0x00
#define nRF24L01_EN_AA        0x01  /*Auto ack*/
#define nRF24L01_EN_RXADDR    0x02
#define nRF24L01_SETUP_AW     0x03
#define nRF24L01_SETUP_RETR   0x04
#define nRF24L01_RF_CH        0x05
#define nRF24L01_RF_SETUP     0x06
#define nRF24L01_STATUS       0x07
#define nRF24L01_OBSERVE_TX   0x08
#define nRF24L01_CD           0x09
#define nRF24L01_RX_ADDR_P0   0x0A
#define nRF24L01_RX_ADDR_P1   0x0B
#define nRF24L01_RX_ADDR_P2   0x0C
#define nRF24L01_RX_ADDR_P3   0x0D
#define nRF24L01_RX_ADDR_P4   0x0E
#define nRF24L01_RX_ADDR_P5   0x0F
#define nRF24L01_TX_ADDR      0x10
#define nRF24L01_RX_PW_P0     0x11
#define nRF24L01_RX_PW_P1     0x12
#define nRF24L01_RX_PW_P2     0x13
#define nRF24L01_RX_PW_P3     0x14
#define nRF24L01_RX_PW_P4     0x15
#define nRF24L01_RX_PW_P5     0x16
#define nRF24L01_FIFO_STATUS  0x17


//SPI Commands
#define nRF24L01_R_REGISTER           0x00
#define nRF24L01_W_REGISTER           0x20
#define nRF24L01_R_RX_PAYLOAD         0x61
#define nRF24L01_W_TX_PAYLOAD         0xA0
#define nRF24L01_FLUSH_TX             0xE1
#define nRF24L01_FLUSH_RX             0xE2
#define nRF24L01_REUSE_TX_PL          0xE3
#define nRF24L01_R_RX_PL_WID          0x60
#define nRF24L01_W_ACK_PAYLOAD        "1010 1PPP"
#define nRF24L01_W_TX_PAYLOAD_NOACK   0xB0
#define nRF24L01_NOP                  0xFF

void nRF24L01_Init1(void);
void TestSend(void);

#endif