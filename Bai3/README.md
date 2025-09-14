# - Cấu hình ngắt ngoài cho 1 nút nhấn. Khi nhấn nút, trạng thái của LED sẽ đảo ngược. 
# - Trong quá trình đó 1 LED khác vẫn nhấp nháy với chu kỳ 1Hz

## Các bước thực hiện:

**1. Cấu hình LED:**
```c

void Led_Config(){
	GPIO_InitTypeDef 		GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_0 | GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed  = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

```
   - Hàm `Led_Config()` khởi tạo chân PA0 và PA2 ở chế độ xuất (Output Push-Pull) với tốc độ 50MHz để điều khiển LED.


**2. Cấu hình nút nhấn:**
```c

void Button_Config(){
	GPIO_InitTypeDef 		GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin 		= GPIO_Pin_1;

	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

```
   - Hàm `Button_Config()` khởi tạo chân PA1 ở chế độ đầu vào kéo lên (Input Pull-Up) để đọc trạng thái nút nhấn.


**3. Hàm delay:**
```c

void delay(int time){
	volatile int i,j;
	for(i = 0; i<time;i++){
		for(j =0; j<0x2aff;j++);
	}
}

```

   - Hàm `delay(int time)` tạo độ trễ bằng vòng lặp for lồng nhau (delay chỉ tương đối, không chính xác).

**4. Cấu hình ngắt ngoài:**
```c

void EXTI_Config(){
	EXTI_InitTypeDef EXTI_InitStruct;
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);  

	EXTI_InitStruct.EXTI_Line 		= EXTI_Line1;
	EXTI_InitStruct.EXTI_LineCmd 	= ENABLE;
	EXTI_InitStruct.EXTI_Mode 		= EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger 	= EXTI_Trigger_Falling;  

	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_SetPriority(EXTI1_IRQn, 0);
	NVIC_EnableIRQ(EXTI1_IRQn);
}

```
   - Hàm `EXTI_Config()` cấu hình chế độ ngắt ngoài trên line 1 với trigger falling (tạo tín hiệu ngắt khi phát hiện sườn xuống của tín hiệu).
   - Hàm `NVIC_SetPriority(EXTI1_IRQn,0);` và `NVIC_EnableIRQ(EXTI1_IRQn);`: cài đặt thứ tự ưu tiên ngắt và cho phép ngắt.


```c

void EXTI1_IRQHandler(void){
    if(EXTI_GetITStatus(EXTI_Line1) != RESET){
				GPIOA->ODR ^= GPIO_Pin_0;    
        EXTI_ClearITPendingBit(EXTI_Line1); 
    }
}

```
   - Khi nút nhận được nhấn, một tín hiệu cạnh xuống được tạo ra → kích hoạt **ngắt ngoài EXTI1**.
   - Chương trình chuyển tới hàm `EXTI1_IRQHandler()`.
   - Trong hàm xử lý ngắt, LED PA0 sẽ **đổi trạng thái** (ON → OFF hoặc OFF → ON).
   - Cờ ngắt được xóa, vi điều khiển quay lại tiếp tục vòng lặp chính.

**5. Hàm main:**
```c

int main(){
	Led_Config();
	Button_Config();
	EXTI_Config();
	
	while(1){
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);
		delay(500);
		GPIO_SetBits(GPIOA, GPIO_Pin_2);
		delay(500);
	}
}

```
   - Gọi các hàm cấu hình: `Led_Config()`, `Button_Config()`, `EXTI_Config()`
   - Trong vòng lặp `while(1)`, thực hiện nhấp nháy led PA2

## **Video demo: [Watch Demo Video](https://drive.google.com/file/d/1TjXb-lyYYNlh3eN73ybeD6RB5MCCDlpk/view?usp=drive_link)**
