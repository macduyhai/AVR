/**************************************/
#include "stm32f10x_conf.h"
#include <stdio.h>


/**************************************/
#define  FLASH_SIZE  (*((uint16_t*)0x1FFFF7E0))
#define  U_ID_0      (*((uint32_t*)0x1FFFF7E8))
#define  U_ID_1      (*((uint32_t*)0x1FFFF7EC))
#define  U_ID_2      (*((uint32_t*)0x1FFFF7F0))

void PrintSystemInfo()
{
  printf("\n");
  printf("      U_ID = %08X %08X %08X\n", U_ID_2, U_ID_1, U_ID_0);
  printf("FLASH_SIZE = %d KB\n", FLASH_SIZE);
  printf("\n");

  printf("RCC_CR    = %08X\n", RCC->CR);
  printf("RCC_CFGR  = %08X\n", RCC->CFGR);
  printf("FLASH_ACR = %08X\n", FLASH->ACR);
  printf("\n");

  RCC_ClocksTypeDef RCC_ClocksStatus;
  RCC_GetClocksFreq(&RCC_ClocksStatus);
  printf("RCC_SYSCLK_Frequency = %ld\n", RCC_ClocksStatus.SYSCLK_Frequency);
  printf("RCC_HCLK_Frequency   = %ld\n", RCC_ClocksStatus.HCLK_Frequency);
  printf("RCC_PCLK1_Frequency  = %ld\n", RCC_ClocksStatus.PCLK1_Frequency);
  printf("RCC_PCLK2_Frequency  = %ld\n", RCC_ClocksStatus.PCLK2_Frequency);
  printf("RCC_ADCCLK_Frequency = %ld\n", RCC_ClocksStatus.ADCCLK_Frequency);
  printf("\n");
}


/**************************************/
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


/**************************************/
void UART1_Init_A9A10()
{
  // Enable clock for UART1 & GPIOA
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);       // Configure the USART1

  //USART_ITConfig(USART1, USART_IT_TXE, ENABLE);  // TX Empty
  //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  // RX Not Empty

  USART_Cmd(USART1, ENABLE);  // enable UART1

  {
    GPIO_InitTypeDef	gpio_init_struct;

    // GPIOA PIN9 alternative function Tx
    gpio_init_struct.GPIO_Pin = GPIO_Pin_9;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_10MHz;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio_init_struct);

    // GPIOA PIN10 alternative function Rx
    gpio_init_struct.GPIO_Pin = GPIO_Pin_10;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &gpio_init_struct);
  }
}

int _write(int file, char *ptr, int len)
{
  for (int i = len; i != 0; i--)
  {
    while ((USART1->SR & USART_FLAG_TXE) == 0);
    USART1->DR = *ptr++;
  }
  return len;
}


/**************************************/
void LED_B2_config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // Enable clock for GPIOB

	GPIO_InitTypeDef	led_b2_init_struct;
	led_b2_init_struct.GPIO_Pin = GPIO_Pin_0;
	led_b2_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
	led_b2_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &led_b2_init_struct);
}


/**************************************/
int main(void)
{
  DWT_config();
  UART1_Init_A9A10();
  LED_B2_config();
  PrintSystemInfo();

  int counter = 0;

  while (1)
  {
    printf("%x\n", counter++);
    printf("het truyen\n");

    GPIO_SetBits(GPIOB, GPIO_Pin_0);
    delay_ms(500);
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
    delay_ms(500);
  }
}


/**************************************/
