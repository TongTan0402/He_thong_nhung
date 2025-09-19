#include "all_header.h"

uint8_t addr[] = {0xE5, 0xA3, 0xB4, 0xC5, 0xF5};

int main()
{
  Delay_Init();
  uart1.config(115200, REMAP);
  SPI1_Config();
	
  NRF_RX_Mode_Init(addr, 40, 32);

  uint8_t data[5];

  NRF_ReadReg_WithMultiBytes(NRF_REG_RX_ADDR_P0, data, 5);

  uart1.print("\n0x");
  for(int i=0; i<5; i++) uart1.print("%h", data[i]);

  while (1)
  {
    
  }
}