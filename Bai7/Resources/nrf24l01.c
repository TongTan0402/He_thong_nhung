#include "nrf24l01.h"
#include "spi1.h"


#define R_REGISTER  	0x00
#define W_REGISTER  	0x20
#define R_RX_PAYLOAD 	0x61
#define W_TX_PAYLOAD 	0xA0
#define STATUS				0x07

void NRF24L01_TX_Mode_Init(uint8_t *addr, uint8_t channel)
{
	NRF24L01_WriteReg(0x00, 0x0A); // CONFIG: EN_CRC = 1, PWR_UP=1
	NRF24L01_WriteReg(0x01, 0x01); // EN_AA: auto-ack
	NRF24L01_WriteReg(0x02, 0x01); // EN_RXADDR: enable pipe0
	NRF24L01_WriteReg(0x03, 0x03); // SETUP_AW: 5 bytes addr
	NRF24L01_WriteReg(0x04, 0x3f); // SETUP_RETR: 1500us, 15 retries
	NRF24L01_WriteReg(0x05, channel & 0x7F);   // RF_CH: channel
	NRF24L01_WriteReg(0x06, 0x07); // RF_SETUP: 1Mbps, 0dBm
	
	NRF24L01_CSN_LOW();
	SPI1_ReadWrite(W_REGISTER | 0x10); // TX_ADDR
	for (int i=0;i<5;i++) SPI1_ReadWrite(addr[i]);
	NRF24L01_CSN_HIGH();
	
	// RX_ADDR_P0 phải trùng với địa chỉ TX để Auto-ACK hoạt động
	NRF24L01_CSN_LOW();
	SPI1_ReadWrite(W_REGISTER | 0x0A); // RX_ADDR_P0
	for (int i=0;i<5;i++) SPI1_ReadWrite(addr[i]);
	NRF24L01_CSN_HIGH();

	// Clear cờ trong STATUS
	NRF24L01_WriteReg(0x07, 0x70);

	// TX mode: CE = 0, chỉ pulse khi gửi
	NRF24L01_CE_LOW();
}

void NRF24L01_RX_Mode_Init(uint8_t *addr, uint8_t channel)
{
	NRF24L01_WriteReg(0x00, 0x0B); // CONFIG: EN_CRC = 1, PWR_UP=1, PRIM_RX=1
	NRF24L01_WriteReg(0x01, 0x01); // EN_AA: auto-ack
	NRF24L01_WriteReg(0x02, 0x01); // EN_RXADDR: enable pipe0
	NRF24L01_WriteReg(0x03, 0x03); // SETUP_AW: 5 bytes addr
	NRF24L01_WriteReg(0x04, 0x04); // SETUP_RETR: 1500us, 15 retries
	NRF24L01_WriteReg(0x05, channel & 0x7F);   // RF_CH: channel
	NRF24L01_WriteReg(0x06, 0x07); // RF_SETUP: 1Mbps, 0dBm
	
	NRF24L01_CSN_LOW();
	SPI1_ReadWrite(W_REGISTER | 0x0A); // RX_ADDR_P0
	for (int i=0;i<5;i++) SPI1_ReadWrite(addr[i]);
	NRF24L01_CSN_HIGH();
	
	// Set payload width cho pipe0 (5 byte)
	NRF24L01_WriteReg(0x11, 5);  

	// Clear interrupt flags trong STATUS
	NRF24L01_WriteReg(0x07, 0x70);

	// CE = 1 để bắt đầu lắng nghe
	NRF24L01_CE_HIGH();
}

void NRF24L01_WriteReg(uint8_t reg, uint8_t value) 
{
	NRF24L01_CSN_LOW();
	SPI1_ReadWrite(W_REGISTER | reg);
	SPI1_ReadWrite(value);
	NRF24L01_CSN_HIGH();
}

uint8_t NRF24L01_ReadReg(uint8_t reg) 
{
	NRF24L01_CSN_LOW();
	SPI1_ReadWrite(R_REGISTER | reg);
	uint8_t data = SPI1_ReadWrite(0xff);
	NRF24L01_CSN_HIGH();
	return data;
}

void NRF24L01_SendData(uint8_t *data, uint8_t len) 
{
	NRF24L01_CSN_LOW();
	SPI1_ReadWrite(0xA0); // W_TX_PAYLOAD
	for (int i=0;i<len;i++) 
	{
		SPI1_ReadWrite(data[i]);
	}
	NRF24L01_CSN_HIGH();

	// Pulse CE để phát
	NRF24L01_CE_HIGH();
	for (volatile int d=0; d<1000; d++); // delay ngắn ~10us
	NRF24L01_CE_LOW();

	// Đợi TX_DS hoặc MAX_RT
	uint8_t status;
	do 
	{
		NRF24L01_CSN_LOW();
		status = SPI1_ReadWrite(0xFF); // đọc STATUS
		NRF24L01_CSN_HIGH();

	} while (!(status & (0x20 | 0x10))); // TX_DS hoặc MAX_RT

	// Xóa cờ
	NRF24L01_WriteReg(STATUS, status & (0x20 | 0x10));
}

void NRF24L01_StartListening(void)
{
	NRF24L01_CE_HIGH();
}

void NRF24L01_StopListening(void)
{
	NRF24L01_CE_LOW();
}


uint8_t NRF24L01_DataReady(void) 
{
	uint8_t status;
	NRF24L01_CSN_LOW();
	status = SPI1_ReadWrite(0xFF); // đọc STATUS
	NRF24L01_CSN_HIGH();
	return status & 0x40; // RX_DR bit6
}

void NRF24L01_ReadData(uint8_t *data, uint8_t len) 
{
	NRF24L01_CSN_LOW();
	SPI1_ReadWrite(R_RX_PAYLOAD); // R_RX_PAYLOAD
	for (int i=0;i<len;i++) {
			data[i] = SPI1_ReadWrite(0xFF);
	}
	NRF24L01_CSN_HIGH();

	// Xóa cờ RX_DR
	NRF24L01_WriteReg(STATUS, 0x40);
}

