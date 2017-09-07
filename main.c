#include "stm32f10x_conf.h"

//chân ST_CP của 74HC595
int latchPin = GPIO_Pin_1;
//chân SH_CP của 74HC595
int clockPin = GPIO_Pin_2;
//Chân DS của 74HC595
int dataPin = GPIO_Pin_3;
//Trạng thái của LED, hay chính là byte mà ta sẽ gửi qua shiftOut
uint8_t ledStatus=0;
/*******************************/

void LED_A0_config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // Enable clock for GPIOA

	GPIO_InitTypeDef	led_a0_init_struct;
	led_a0_init_struct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2|GPIO_Pin_3;
	led_a0_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
	led_a0_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &led_a0_init_struct);
}

/*****************************/
void DWT_config(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  // enable Debug block
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;  // enable DWT counter
}


void delay_us(uint32_t micros)
{
  uint32_t t0 = DWT->CYCCNT;
  uint32_t delta = 72 * micros;
  while ((DWT->CYCCNT - t0) < delta);
}

void delay_ms(unsigned long ms)
{
  while (ms--)
    delay_us(1000);
}

uint64_t dwt_CycleCount64()
{
  static uint64_t last_cycle_count_64 = 0;
  last_cycle_count_64 += DWT->CYCCNT - (uint32_t)(last_cycle_count_64);
  return last_cycle_count_64;
}

uint64_t dwt_Millis()
{
  return dwt_CycleCount64() / 72000;
}

uint64_t dwt_Micros()
{
  return dwt_CycleCount64() / 72;
}

//////////\\\\\\\\\\\\\\\

 void my_shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
  uint8_t i;
  GPIO_WriteBit(GPIOA,clockPin,0);
  for (i = 0; i < 8; i++)  {
    if (bitOrder == 0)
      GPIO_WriteBit(GPIOA,dataPin, !!(val & (1 << i)));    //khong can co !!
    else
      GPIO_WriteBit(GPIOA,dataPin, !!(val & (1 << (7 - i))));

    GPIO_WriteBit(GPIOA,clockPin,1);
    GPIO_WriteBit(GPIOA,clockPin,0);

  }
  GPIO_WriteBit(GPIOA,latchPin,1);
  GPIO_WriteBit(GPIOA,latchPin,0);
}
void sangtuantu()
{
 //Sáng tuần tự
  ledStatus = 0;//mặc định là không có đèn nào sáng hết (0 = 0b00000000)
  for (int i = 0; i < 8; i++) {
    ledStatus = (ledStatus << 1) | 1;//Đẩy toàn bộ các bit qua trái 1 bit và cộng bit có giá trị là 1 ở bit 0
    GPIO_ResetBits(GPIOA,latchPin); //các đèn LED sẽ không sáng khi bạn digital LOW
    my_shiftOut(dataPin, clockPin, 1, ledStatus);
    GPIO_WriteBit(GPIOA,latchPin,1);//các đèn LED sẽ sáng với trạng thái vừa được cập nhập
    GPIO_WriteBit(GPIOA,latchPin,0);  // output enable
    delay_ms(500); // Dừng chương trình khoảng 500 mili giây để thấy các hiệu ứng của đèn LED
  }
}
void batled(int i)
 {
  ledStatus |= (1<<i);
 // GPIO_ResetBits(GPIOA,latchPin); //các đèn LED sẽ không sáng khi bạn digital LOW
  my_shiftOut(dataPin, clockPin, 1, ledStatus);
  GPIO_WriteBit(GPIOA,latchPin,1);//các đèn LED sẽ sáng với trạng thái vừa được cập nhập
  GPIO_WriteBit(GPIOA,latchPin,0);  // output enable
  delay_ms(500);
 }
 void tatled1(int i)
 {
  ledStatus &= ~(1<<i);
 // GPIO_ResetBits(GPIOA,latchPin); //các đèn LED sẽ không sáng khi bạn digital LOW
  my_shiftOut(dataPin, clockPin, 1, ledStatus);
  GPIO_WriteBit(GPIOA,latchPin,1);//các đèn LED sẽ sáng với trạng thái vừa được cập nhập
  GPIO_WriteBit(GPIOA,latchPin,0);  // output enable
  delay_ms(500);
 }
 void tatled()
 {
  my_shiftOut(dataPin, clockPin, 1, 0);
  GPIO_WriteBit(GPIOA,latchPin,1);//các đèn LED sẽ sáng với trạng thái vừa được cập nhập
  GPIO_WriteBit(GPIOA,latchPin,0);
  delay_ms(100);
  ledStatus=0;
 }
 void setled(int pos, int state)
 {
  //if(state==1) ledStatus |= (1<<pos);
  //else  ledStatus &= ~(1<<pos);

  //ledStatus = ledStatus & ~(!state<<pos) | (state << pos);
   ledStatus = (ledStatus & (~(1<<pos))) | (state << pos);

  my_shiftOut(dataPin, clockPin, 1, ledStatus);
  GPIO_WriteBit(GPIOA,latchPin,1);//các đèn LED sẽ sáng với trạng thái vừa được cập nhập
  GPIO_WriteBit(GPIOA,latchPin,0);  // output enable
  delay_ms(500);
 }
/*******************************/
int main(void)
{
  DWT_config();

  LED_A0_config();



  while (1)
  {
  //sangtuantu();
  for(int i=0;i<8;i++)
  {
    setled(i,1);
    delay_ms(200);
  }
  for(int i=7;i>=0;i--)
  {
    setled(i,0);
    delay_ms(200);
  }
  //tatled();
  }

  //tatled();
  delay_ms(500);


}// end main


/*******************************/

