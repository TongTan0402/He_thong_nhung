#include "all_header.h"

int main()
{
  Delay_Init();
//  uart1.config(115200, NO_REMAP);
  SPI1_Config();
	uint8_t addr[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
  NRF24L01_TX_Mode_Init(addr, 40);

  uint8_t config_reg = NRF24L01_ReadReg(0x00);

  while (1)
  {
    
  }
}