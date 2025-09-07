# Mô tả chương trình điều khiển LED trên STM32F10x

Chương trình này thực hiện việc điều khiển toàn bộ các chân GPIO của hai cổng A và B trên vi điều khiển STM32F10x để bật/tắt LED theo chu kỳ.

## Các bước thực hiện:

**1. Cấu hình GPIO:**
```c
  void led_config(){
    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    gpio.GPIO_Mode=GPIO_Mode_Out_PP;
    gpio.GPIO_Pin=GPIO_Pin_All;
    gpio.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init (GPIOA,&gpio);
    GPIO_Init (GPIOB,&gpio);
  }
```
   - Hàm `led_config()` khởi tạo toàn bộ các chân của GPIOA và GPIOB ở chế độ xuất (Output Push-Pull) với tốc độ 50MHz:
     - Kích hoạt xung clock cho hai cổng GPIOA và GPIOB.
     - Cấu hình tất cả các chân của GPIOA và GPIOB thành chế độ Output push-pull.
     - Clock speed được đặt là 50MHz.

**2. Hàm delay:**

```c
    
  void delay (unsigned int t){
    int i,j;
    for(i=0;i<t;i++){
      for(j=0;j<0x2aff;j++);
    }
  }

```
   - Hàm `delay(unsigned int t)` tạo độ trễ bằng cách sử dụng vòng lặp for lồng nhau.

**3. Vòng lặp chính:**
```c
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
```
   - Trong hàm `main()`, chương trình liên tục bật tất cả các chân của GPIOA và GPIOB (`ODR=0xff`), sau đó tắt tất cả các chân (`ODR=0x00`) với chu kỳ delay 500ms (tương đối, không chính xác), tạo thành một hiệu ứng nhấp nháy cho các LED với chu kỳ 1000ms.

## Mục đích:
- Bật/tắt toàn bộ các LED được kết nối với các chân của hai cổng GPIOA và GPIOB theo chu kỳ 1000ms

## Lưu ý:
- Đảm bảo các chân GPIOA và GPIOB đã được kết nối với LED và có điện trở hạn dòng phù hợp.
- Độ trễ có thể thay đổi tùy theo yêu cầu và có thể không chính xác do sử dụng vòng lặp for để tạo độ trễ.