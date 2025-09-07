# Mô tả chương trình điều khiển LED bằng nút nhấn trên STM32F10x

Chương trình này thực hiện việc điều khiển trạng thái LED thông qua nút nhấn sử dụng vi điều khiển STM32F10x.

## Các bước thực hiện:

**1. Cấu hình LED:**
```c

void config_led(){
  GPIO_InitTypeDef gpio;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Pin = GPIO_Pin_0;         
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &gpio);
}

```
   - Hàm `config_led()` khởi tạo chân PA0 của GPIOA ở chế độ xuất (Output Push-Pull) với tốc độ 50MHz để điều khiển LED.

**2. Cấu hình nút nhấn:**
   - Hàm `config_button()` khởi tạo chân PB0 của GPIOB ở chế độ đầu vào kéo lên (Input Pull-Up)để đọc trạng thái nút nhấn.

```c
void config_button(){
  GPIO_InitTypeDef gpio;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  gpio.GPIO_Mode = GPIO_Mode_IPU;
  gpio.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOB, &gpio);
}

```
**3. Hàm delay:**
   - Hàm `delay(unsigned int t)` tạo độ trễ bằng vòng lặp for lồng nhau (delay chỉ tương đối, không chính xác).

**4. Vòng lặp chính:**
   - Biến `cnt` dùng để đếm số lần nhấn nút.
```c
int main(){
  config_led();
  config_button();
  while(1){
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0){
      cnt++;
      while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0);
    }
    if(cnt % 2 == 0){
      GPIO_SetBits(GPIOA, GPIO_Pin_0);
    } else {
      GPIO_ResetBits(GPIOA, GPIO_Pin_0);
    }
  }
}

```
   - Khi nút nhấn PB0 được nhấn (mức thấp), biến `cnt` tăng lên và chờ đến khi nút được nhả ra (chống dội phím).
   - Nếu số lần nhấn là lẻ, LED tắt (PA0 mức thấp). Nếu số lần nhấn là chẵn, LED sáng (PA0 mức cao).

## Mục đích:

- Điều khiển trạng thái LED (bật/tắt) bằng cách nhấn nút PB0. Mỗi lần nhấn nút sẽ đảo trạng thái LED.

## Lưu ý:

- Đảm bảo chân PA0 kết nối với LED và PB0 kết nối với nút nhấn, có điện trở hạn dòng phù hợp.
- Độ trễ trong hàm delay chỉ mang tính chất tương đối, có thể điều chỉnh cho phù hợp với yêu cầu thực tế.