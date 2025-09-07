#include "stm32f10x.h"                  // Device header

void delay (unsigned int t){
	int i,j;
	for(i=0;i<t;i++){
		for(j=0;j<0x2aff;j++);
	}
}

void led_config(){
	GPIO_InitTypeDef gpio;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	gpio.GPIO_Mode=GPIO_Mode_Out_PP;
	gpio.GPIO_Pin=GPIO_Pin_All;
	gpio.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init (GPIOA,&gpio);
	GPIO_Init (GPIOB,&gpio);
}

int main(){
	led_config();
	while(1){
		GPIOA->ODR=0xff;
		GPIOB->ODR=0xff;
		delay(500);
		GPIOA->ODR=0x00;
		GPIOB->ODR=0x00;
		delay(500);
	}
}