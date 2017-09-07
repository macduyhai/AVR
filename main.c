#include "stm32f10x_conf.h"

int led[8]={GPIO_Pin_0,GPIO_Pin_1,GPIO_Pin_2,GPIO_Pin_3,GPIO_Pin_4,GPIO_Pin_5,GPIO_Pin_6,GPIO_Pin_7};

/*******************************/

void LED_A0_config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // Enable clock for GPIOA

	GPIO_InitTypeDef	led_a0_init_struct;
	led_a0_init_struct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1 | GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
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
void giot_nuoc()
{
  int dem=7;
  int t=dem;

  while(dem!=0)
  {
    for (int i=0;i<dem;i++)
    {
      GPIO_SetBits(GPIOA, led[i]);
      delay_ms(100);
      GPIO_ResetBits(GPIOA, led[i]);
      //delay_ms(100);
    }
    GPIO_SetBits(GPIOA, led[dem-1]);
    delay_ms(100);
    dem--;

  }
  for( int i=0;i<t;i++){
   GPIO_ResetBits(GPIOA, led[i]);
  }
  delay_ms(100);
}
 void traiphai()
 {
  int dem=7;
  for(int i=0;i<dem;i++)
  {
   GPIO_SetBits(GPIOA,led[i]);
   delay_ms(100);
   GPIO_ResetBits(GPIOA, led[i]);
   delay_ms(100);
  }
  for(int i=dem-1;i>=0;i--)
  {
   GPIO_SetBits(GPIOA,led[i]);
   delay_ms(100);
   GPIO_ResetBits(GPIOA, led[i]);
   delay_ms(100);
  }
 }
 void testled()
 {
  GPIO_SetBits(GPIOA, GPIO_Pin_2);
    delay_ms(100);
    GPIO_ResetBits(GPIOA, GPIO_Pin_2);
    delay_ms(100);
 }
 void batled(int i)
 {

 }

/*******************************/
int main(void)
{
  DWT_config();

  LED_A0_config();



  while (1)
  {

  //giot_nuoc();
  //delay_ms(100);
  //traiphai();
  //delay_ms(100);
  testled();
   }
} // end main


/*******************************/

