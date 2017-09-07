#include "stm32f10x_conf.h"

//chân ST_CP của 74HC595
int latchPin = GPIO_Pin_1;
//chân SH_CP của 74HC595
int clockPin = GPIO_Pin_2;
//Chân DS của 74HC595
int dataPin = GPIO_Pin_3;
//Trạng thái của LED, hay chính là byte mà ta sẽ gửi qua shiftOut
uint16_t ledStatus=0;
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

 void my_shiftOut16(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint16_t val)
{
  uint16_t i;
  GPIO_WriteBit(GPIOA,clockPin,0);
  for (i = 0; i < 16; i++)  {
    if (bitOrder == 0)
      GPIO_WriteBit(GPIOA,dataPin, !!(val & (1 << i)));    //khong can co !!
    else
      GPIO_WriteBit(GPIOA,dataPin, !!(val & (1 << (15 - i))));

    GPIO_WriteBit(GPIOA,clockPin,1);
    GPIO_WriteBit(GPIOA,clockPin,0);

  }
  GPIO_WriteBit(GPIOA,latchPin,1);
  GPIO_WriteBit(GPIOA,latchPin,0);
}


 void tatled()
 {
  my_shiftOut16(dataPin, clockPin, 1, 0);
  delay_ms(100);
  ledStatus=0;
 }
 void setled(int pos, int state)
 {
  ledStatus = (ledStatus & (~(1<<pos))) | (state << pos);
  my_shiftOut16(dataPin, clockPin, 1, ledStatus);

  delay_ms(500);
 }
/*******************************/
int main(void)
{
  DWT_config();

  LED_A0_config();



  while (1)
  {
//   //////bat tat led//////
//
//  for(int i=0;i<16;i++)
//  {
//    setled(i,1);
//    delay_ms(20);
//  }
//  for(int i=15;i>=0;i--)
//  {
//    setled(i,0);
//    delay_ms(20);
//  }

//////////////////

  my_shiftOut16(dataPin,clockPin,1,0xff00);
  delay_ms(100);
  my_shiftOut16(dataPin,clockPin,1,0x00ff);
  delay_ms(100);
  //tatled();
  }
}// end main


/*******************************/

