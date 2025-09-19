#ifndef __NRF24L01__
#define __NRF24L01__
#ifdef __cplusplus
extern "C"{
#endif
#include "stm32f10x.h"                  // Device header

#define R_REGISTER  0x00
#define W_REGISTER  0x20
#define R_RX_PAYLOAD 0

void NRF24L01_TX_Mode_Init(uint8_t *addr, uint8_t channel);
void NRF24L01_RX_Mode_Init(uint8_t *addr, uint8_t channel);
void NRF24L01_WriteReg(uint8_t reg, uint8_t value);
uint8_t NRF24L01_ReadReg(uint8_t reg);
void NRF24L01_SendData(uint8_t *data, uint8_t len);
void NRF24L01_StartListening(void);
void NRF24L01_StopListening(void);
uint8_t NRF24L01_DataReady(void);
void NRF24L01_ReadData(uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif
#endif