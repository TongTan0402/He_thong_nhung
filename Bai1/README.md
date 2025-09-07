# **CÁCH TẠO PROJECT TRÊN KEILC**
- Mở phần mềm KeilC
- Vào `Project -> New Project`  
- Chọn thư mục lưu project và đặt tên cho project
- Chọn loại vi điều khiển (ví dụ: STM32F103C8)
- Vào `Project -> Manage -> Run-Time Environment`
- Chọn các thư viện cần thiết (ví dụ: GPIO, USART, etc.)
- Nhấn OK để hoàn tất việc thêm thư viện
- Vào `Project -> Options for Target 'Target 1'`
- Cấu hình các thông số như: Clock, Debug, Output, etc.
- Vào `File -> New` để tạo file mã nguồn mới
- Lưu file với phần mở rộng `.c` hoặc `.h` (ví dụ: `main.c`)
- Viết mã nguồn trong file vừa tạo
- Vào `Project -> Build Target` để biên dịch mã nguồn
- Kiểm tra lỗi trong cửa sổ Output
- Kết nối vi điều khiển với máy tính qua cáp lập trình
- Vào `Flash -> Download` để nạp mã nguồn vào vi điều khiển

# **LẬP TRÌNH NHẤP NHÁY LED PC13 TRÊN STM32F103C8T6**
- Cấu hình GPIO cho chân PC13
- Viết hàm khởi tạo GPIO:
```c
#include "stm32f10x.h"

void gpio_config(){
	GPIO_InitTypeDef gpio;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	gpio.GPIO_Mode=GPIO_Mode_Out_PP;
	gpio.GPIO_Pin=GPIO_Pin_13;
	gpio.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init (GPIOC, &gpio);
}
```

- Viết hàm delay đơn giản dùng vòng lặp:
```c

void delay(unsigned int t){
	unsigned i,j;
	for(i=0;i<t;i++){
		for(j=0;j<0x2aff;j++);
	}
}
	
```

- Viết hàm main để nhấp nháy LED:
```c 

int main(){
		gpio_config();
	while(1){
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
		delay(500);
		GPIO_SetBits(GPIOC,GPIO_Pin_13);
		delay(500);
	}
}

```

- Video demo: [Watch Demo Video](https://drive.google.com/file/d/1KL8eQFrpdIDMnKsJEWwD6RB7-p9vwk13/view?usp=sharing)
