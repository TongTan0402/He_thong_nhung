#include "nrf24l01.h"
#include "spi1.h"

void NRF_WriteReg_WithOneByte(uint8_t reg, uint8_t value) 
{
	NRF_CSN_LOW();
	SPI1_ReadWrite(NRF_CMD_W_REGISTER | reg);
	SPI1_ReadWrite(value);
	NRF_CSN_HIGH();
}

uint8_t NRF_ReadReg_WithOneByte(uint8_t reg) 
{
	NRF_CSN_LOW();
	SPI1_ReadWrite(NRF_CMD_R_REGISTER | reg);
	uint8_t data = SPI1_ReadWrite(0xff);
	NRF_CSN_HIGH();
	return data;
}

void NRF_WriteReg_WithMultiBytes(uint8_t reg, uint8_t *data, uint8_t len) 
{
	NRF_CSN_LOW();
	SPI1_ReadWrite(NRF_CMD_W_REGISTER | reg);
	while(len--) SPI1_ReadWrite(*data++);
	NRF_CSN_HIGH();
}

uint8_t NRF_ReadReg_WithMultiBytes(uint8_t reg, uint8_t *data, uint8_t len) 
{
	NRF_CSN_LOW();
	SPI1_ReadWrite(NRF_CMD_R_REGISTER | reg);
	while(len--) *data++ = SPI1_ReadWrite(0xff);
	NRF_CSN_HIGH();
	return data;
}

/*------------------------------------------ TX Mode ------------------------------------------*/
#ifdef TX_MODE

void NRF_TX_Mode_Init(uint8_t *addr, uint8_t channel)
{
	NRF_WriteReg_WithOneByte(NRF_REG_CONFIG, 				CONFIG_EN_CRC | CONFIG_PWR_UP); // CONFIG: EN_CRC = 1, PWR_UP=1
	NRF_WriteReg_WithOneByte(NRF_REG_EN_AA, 				ENAA_P0); // EN_AA: auto-ack
	NRF_WriteReg_WithOneByte(NRF_REG_EN_RXADDR, 		ERX_P0); // EN_RXADDR: enable pipe0
	NRF_WriteReg_WithOneByte(NRF_REG_SETUP_AW, 			AW_5_BYTES); // SETUP_AW: 5 bytes addr
	NRF_WriteReg_WithOneByte(NRF_REG_SETUP_RETR, 		0x3f); // SETUP_RETR: 1000us, 15 retries
	NRF_WriteReg_WithOneByte(NRF_REG_RF_CH, 				channel & 0x7F);   // RF_CH: channel
	NRF_WriteReg_WithOneByte(NRF_REG_RF_SETUP, 			0x07); // RF_SETUP: 1Mbps, 0dBm
	
	NRF_CSN_LOW();
	NRF_WriteReg_WithMultiBytes(NRF_REG_TX_ADDR, addr, 5); 
	NRF_CSN_HIGH();
	
	// RX_ADDR_P0 phải trùng với địa chỉ TX để Auto-ACK hoạt động
	NRF_CSN_LOW();
	NRF_WriteReg_WithMultiBytes(NRF_REG_RX_ADDR_P0, addr, 5); 
	NRF_CSN_HIGH();

	// Clear cờ trong STATUS
	NRF_WriteReg_WithOneByte(NRF_REG_STATUS, 		STATUS_RX_DR | STATUS_TX_DS | STATUS_MAX_RT);

	// TX mode: CE = 0, chỉ pulse khi gửi
	NRF_CE_LOW();
}

void NRF_SendData(uint8_t *data, uint8_t len) 
{
	NRF_CSN_LOW();
	SPI1_ReadWrite(NRF_CMD_W_TX_PAYLOAD);
	for (int i=0;i<len;i++) 
	{
		SPI1_ReadWrite(data[i]);
	}
	NRF_CSN_HIGH();

	// Pulse CE để phát
	NRF_CE_HIGH();
	for (volatile int d=0; d<1000; d++); // delay ngắn ~10us
	NRF_CE_LOW();

	// Đợi TX_DS hoặc MAX_RT
	uint8_t status;
	do 
	{
		NRF_CSN_LOW();
		status = SPI1_ReadWrite(0xFF); // đọc STATUS
		NRF_CSN_HIGH();

	} while (!(status & (STATUS_TX_DS | STATUS_MAX_RT))); // TX_DS hoặc MAX_RT

	// Xóa cờ
	NRF_WriteReg_WithOneByte(STATUS, status & (STATUS_TX_DS | STATUS_MAX_RT));
}

/*------------------------------------------ RX Mode ------------------------------------------*/
#else

void NRF_RX_Mode_Init(uint8_t *addr, uint8_t channel, uint8_t size_of_payload)
{
	NRF_WriteReg_WithOneByte(NRF_REG_CONFIG,				CONFIG_EN_CRC | CONFIG_PWR_UP | CONFIG_PRIM_RX); // CONFIG: EN_CRC = 1, PWR_UP=1, PRIM_RX=1
	NRF_WriteReg_WithOneByte(NRF_REG_EN_AA,					ENAA_P0); // EN_AA: auto-ack
	NRF_WriteReg_WithOneByte(NRF_REG_EN_RXADDR,			ERX_P0); // EN_RXADDR: enable pipe0
	NRF_WriteReg_WithOneByte(NRF_REG_SETUP_AW,			AW_5_BYTES); // SETUP_AW: 5 bytes addr
	NRF_WriteReg_WithOneByte(NRF_REG_SETUP_RETR,		0x3f); // SETUP_RETR: 1000us, 15 retries
	NRF_WriteReg_WithOneByte(NRF_REG_RF_CH,					channel & 0x7F);   // RF_CH: channel
	NRF_WriteReg_WithOneByte(NRF_REG_RF_SETUP,			0x07); // RF_SETUP: 1Mbps, 0dBm
	
	NRF_CSN_LOW();
	NRF_WriteReg_WithMultiBytes(NRF_REG_RX_ADDR_P0, addr, 5); 
	NRF_CSN_HIGH();
	
	// Set payload width cho pipe0 (32 byte)
	size_of_payload = size_of_payload > 32 ? 32 : size_of_payload;
	NRF_WriteReg_WithOneByte(NRF_REG_RX_PW_P0, size_of_payload);  

	// Clear interrupt flags trong STATUS
	NRF_WriteReg_WithOneByte(NRF_REG_STATUS,  	(STATUS_RX_DR | STATUS_TX_DS | STATUS_MAX_RT));

	NRF_StopListening();
}

void NRF_StartListening(void)
{
	NRF_CE_HIGH();
}

void NRF_StopListening(void)
{
	NRF_CE_LOW();
}


uint8_t NRF_DataReady(void) 
{
	uint8_t status;
	NRF_CSN_LOW();
	status = SPI1_ReadWrite(0xFF);
	NRF_CSN_HIGH();
	return status & STATUS_RX_DR;
}

void NRF_ReadData(uint8_t *data, uint8_t len) 
{
	NRF_CSN_LOW();
	SPI1_ReadWrite(NRF_CMD_R_RX_PAYLOAD);
	for (int i=0;i<len;i++) {
			data[i] = SPI1_ReadWrite(0xFF);
	}
	NRF_CSN_HIGH();

	// Xóa cờ RX_DR
	NRF_WriteReg_WithOneByte(NRF_REG_STATUS, STATUS_RX_DR);
}

#endif
