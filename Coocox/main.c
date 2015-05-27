/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "5110.h"
#include "main.h"


#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"

/* Private typedef -----------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static volatile uint32_t TimingDelay;
/* Private function prototypes -----------------------------------------------*/
void Delay(volatile uint32_t nCount);
void TimingDelay_Decrement(void);
/* Private functions ---------------------------------------------------------*/

void setup_usart() {
	// wlaczenie taktowania wybranego portu
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	// wlaczenie taktowania wybranego uk³adu USART
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	// konfiguracja linii Tx
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	// konfiguracja linii Rx
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;
	// predkosc transmisji (mozliwe standardowe opcje: 9600, 19200, 38400, 57600, 115200, ...)
	USART_InitStructure.USART_BaudRate = 9600;
	// d³ugoœæ s³owa (USART_WordLength_8b lub USART_WordLength_9b)
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// liczba bitów stopu (USART_StopBits_1, USART_StopBits_0_5, USART_StopBits_2, USART_StopBits_1_5)
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// sprawdzanie parzystoœci (USART_Parity_No, USART_Parity_Even,	USART_Parity_Odd)
	USART_InitStructure.USART_Parity = USART_Parity_No;
	// sprzêtowa kontrola przep³ywu (USART_HardwareFlowControl_None, USART_HardwareFlowControl_RTS, USART_HardwareFlowControl_CTS, USART_HardwareFlowControl_RTS_CTS)
	USART_InitStructure.USART_HardwareFlowControl =	USART_HardwareFlowControl_None;
	// tryb nadawania/odbierania (USART_Mode_Rx, USART_Mode_Rx )
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// konfiguracja
	USART_Init(USART3, &USART_InitStructure);

	// wlaczenie ukladu USART
	USART_Cmd(USART3, ENABLE);
}

void setup_sterowanie() {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef  Sterowanie;
	Sterowanie.GPIO_Pin = GPIO_Pin_7, GPIO_Pin_6, GPIO_Pin_5, GPIO_Pin_4;
	Sterowanie.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOA, &Sterowanie);
}

void setup_diody() {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef  Diody;
	Diody.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	Diody.GPIO_Mode = GPIO_Mode_OUT;
	Diody.GPIO_OType = GPIO_OType_PP;
	Diody.GPIO_Speed = GPIO_Speed_100MHz;
	Diody.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &Diody);
}

//menu
char screenRows[3][6][15] = {
		{
				"devolay z     ",
				"szynka i serem",
				"filet drobiowy",
				"bukiet surowek",
				"stek wolowy z ",
				"talarkami ziem"
		},
		{
				"shoarma z     ",
				"kurczaka      ",
				"losos na      ",
				"szpinaku      ",
				"salatka cezar ",
				"z kurczakiem  "
		},
		{
				"watrobka z    ",
				"pieczarkami   ",
				"______________",
				">>ZATWIERDZ<<<",
				"______________",
				">>ANULUJ<<<<<<"
		}

};
//Current Menu Level
int cml = 0;

int counter = 0;

void screen(int row) {
	LCD5110_clear();

	if(row==0) LCD5110_write_string_inv(screenRows[cml][0]);
	else LCD5110_write_string(screenRows[cml][0]);

	if(row==0) LCD5110_write_string_inv(screenRows[cml][1]);
	else LCD5110_write_string(screenRows[cml][1]);

	if(row==1) LCD5110_write_string_inv(screenRows[cml][2]);
	else LCD5110_write_string(screenRows[cml][2]);

	if(row==1) LCD5110_write_string_inv(screenRows[cml][3]);
	else LCD5110_write_string(screenRows[cml][3]);

	if(row==2) LCD5110_write_string_inv(screenRows[cml][4]);
	else LCD5110_write_string(screenRows[cml][4]);

	if(row==2) LCD5110_write_string_inv(screenRows[cml][5]);
	else LCD5110_write_string(screenRows[cml][5]);
}

int main(void) {
	if (SysTick_Config(SystemCoreClock / 1000))
		while (1) ;

	setup_diody();
	setup_usart();
	setup_sterowanie();

	int i=0;
	LCD5110_init();
	LCD5110_set_XY(0, 0);
	screen(i);

	int levels = sizeof(screenRows) / sizeof(screenRows[0]);
	while (1) {
		//DOWN
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)!=0){
			if(i+1==3) {
				if(cml+1==levels) {
					i=1;
					counter--;
				}
				else {
					i=-1;
					cml++;
				}
			}
			screen(++i);
			counter++;
			Delay(10);
		}
		//UP
		else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6)!=0){
			if(i-1==-1) {
				if(cml-1<0) {
					i=1;
					counter++;
				}
				else {
					i=3;
					cml--;
				}
			}
			screen(--i);
			counter--;
			Delay(10);
		}
		//OK
		else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)!=0){
			GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
			USART_SendData(USART3, counter);
			while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET) {}
			Delay(40);
		}
		//BACK
		else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4)!=0){
			GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
			USART_SendData(USART3, 100+counter);
			while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET) {}
			Delay(40);
		}
	}
}
void SysTick_Handler(void) {
	TimingDelay_Decrement();
}

void Delay(volatile uint32_t nTime) {
	TimingDelay = nTime;
	while (TimingDelay != 0)
		;
}

void TimingDelay_Decrement(void) {
	if (TimingDelay > 0x00) {
		TimingDelay--;
	}
}
