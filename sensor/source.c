#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_adc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define Attention            0
#define SetClient            1
#define SetSingleConnection  2
#define ConnectToModem       3
#define ConnectToServer      4
#define StartSendingData     5

#define ADC_BufferSize       510
#define ADC_BufferDepth      10



/************************** PRIVATE VARIABLES *************************/

uint8_t  status=0;





uint32_t q=0;
char flag_check=1;
char flag_ESP_Config = 0;
uint8_t ADCSendCounter = 0;

int h;
uint16_t ADCdata=0;
uint8_t ADCbuffer[ADC_BufferDepth][ADC_BufferSize];

uint8_t  ADCsampelling_row = 0;
uint16_t ADCsampelling_col = 0;



char LED_Value;

volatile uint32_t	  SysTickCnt_5ms = 0;



	
UART_CFG_Type uart;      //Uart pointer	
PINSEL_CFG_Type PinCfg;  //Pinsel pointer
UART_FIFO_CFG_Type fifo; //FIFO Pointer

typedef struct{
	uint8_t str[160];
	uint8_t counter;
	char* match;
}espr;

typedef struct{
	uint8_t str[160];
	uint8_t counter;
	
}espt;

static espr EspRx;
static espt EspTx;


char Command[35][100]={
														//"AT+RESTORE\r\n",//0
														//"AT+RST\r\n",
														"AT\r\n",//1
                            "AT+CWMODE=1\r\n",//2
														"AT+CIPMUX=0\r\n",//3
														//"AT+CWJAP=\"ASUS\",\"Arash_Laptop13579\"\r\n",
														"AT+CWJAP=\"Amfoter\",\"terapico0098\"\r\n",//
														//"AT+CWJAP=\"honor7x\",\"terapico0098\"\r\n",
														"AT+CIPSTART=\"TCP\",\"31.184.132.36\",7777,60\r\n",//5
														//"AT+CIPSTART=\"TCP\",\"198.143.182.157\",7777,60\r\n",//5
													//	"AT+CIPSTART=\"TCP\",\"192.168.1.104\",8050,60\r\n",//5
	                          "AT+CIPSENDEX=510\r\n"//6
													};


   
const char Respons[35][30]={
														
														//"OK",//0
                            "OK",//1
                            "OK",//2
                            "OK",//3
                            "OK",//4
                            "OK",//5
                            "OK\r\n>",//6
														"SEND OK"//7
													 };

const char Faults[35][30]={
														//"ERROR",
                            "ERROR",
                            "ERROR",
                            "ERROR",
                            "ERROR",
                            "ERROR",
                            "ERROR",
														"ERROR"
													 };
/************************** PRIVATE FUNCTIONS *************************/
void uart_configuration(void);

void ADC_configuration(void);
uint16_t Read_adc_x(void);
uint16_t Read_adc_y(void);
uint16_t Read_adc_z(void);

void MainTimerInit( uint16_t MainTimer_US_Value);
void Call_5ms(void);
char ESP_Config(void);









int main(void)
{
			
	
		uart_configuration();
	  ADC_configuration();

		GPIO_SetDir(1, 0x000C0000, 1);           /* P1.18 & P1.19 defined as Output    */
		GPIO_ClearValue(1, 0x000C0000);
			MainTimerInit(400);
	while(1)
	{		
		if(flag_ESP_Config == 0)
		{
			while(!ESP_Config());
		}
		else
		{
			if ( SysTickCnt_5ms >= 12 ) {SysTickCnt_5ms = 0; Call_5ms();}
		}
		
	}
}



void MainTimerInit( uint16_t MainTimer_US_Value )
{
	TIM_TIMERCFG_Type TIM_ConfigStruct;
	TIM_MATCHCFG_Type TIM_MatchConfigStruct ;
	// Initialize timer 0, prescale count time of 1uS
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 1;

	// use channel 0, MR0
	TIM_MatchConfigStruct.MatchChannel = 0;
	// Enable interrupt when MR0 matches the value in TC register
	TIM_MatchConfigStruct.IntOnMatch   = TRUE;
	//Enable reset on MR0: TIMER will reset if MR0 matches it
	TIM_MatchConfigStruct.ResetOnMatch = TRUE;
	//Stop on MR0 if MR0 matches it
	TIM_MatchConfigStruct.StopOnMatch  = FALSE;
	//Toggle MR0.0 pin if MR0 matches it
	TIM_MatchConfigStruct.ExtMatchOutputType =TIM_EXTMATCH_NOTHING;
	// Set Match value, count value of 100 (100 * 1uS = 100us --> 10 kHz)
	TIM_MatchConfigStruct.MatchValue   = MainTimer_US_Value;

	// Set configuration for Tim_config and Tim_MatchConfig
	TIM_Init(LPC_TIM1, TIM_TIMER_MODE,&TIM_ConfigStruct);
	TIM_ConfigMatch(LPC_TIM1,&TIM_MatchConfigStruct);

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(TIMER1_IRQn, ((0x01<<3)|0x02));

	/* Enable interrupt for timer 0 & UART0 channel*/
	NVIC_EnableIRQ(TIMER1_IRQn);
	// To start timer 0
	TIM_Cmd(LPC_TIM1,ENABLE);
}


void TIMER1_IRQHandler(void)
{
	
	if (TIM_GetIntStatus(LPC_TIM1, TIM_MR0_INT)== SET)
	{
		SysTickCnt_5ms++;	
		ADCdata = Read_adc_x();
		
		ADCbuffer[ADCsampelling_row][ADCsampelling_col] = ADCdata;
		ADCbuffer[ADCsampelling_row][ADCsampelling_col+1] = ADCdata>>8;

//  		ADCbuffer[ADCsampelling_row][ADCsampelling_col] = 255;
//  		ADCbuffer[ADCsampelling_row][ADCsampelling_col+1] = 15; // 0xFFF
		
//  		ADCbuffer[ADCsampelling_row][ADCsampelling_col] = 0;
//  		ADCbuffer[ADCsampelling_row][ADCsampelling_col+1] = 0; //0x000
		
//  		ADCbuffer[ADCsampelling_row][ADCsampelling_col] = 'z';
//  		ADCbuffer[ADCsampelling_row][ADCsampelling_col+1] = 0; 0x07A

		
		ADCsampelling_col += 2;
		if(ADCsampelling_col > (ADC_BufferSize-12))
		{
			ADCsampelling_col = 0;
			ADCSendCounter = ADCsampelling_row;	
			ADCsampelling_row ++;
		}
		if(ADCsampelling_row > (ADC_BufferDepth-1))
		{
			ADCsampelling_row  = 0;
			
			
			LED_Value ++;
		  GPIO_SetValue(1, ((LED_Value&1)<<19));
		  GPIO_ClearValue(1, (~(LED_Value&1)<<19));

		}

	}
	TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);
	
	
	
		

}









void UART0_IRQHandler(void)
{

	if((LPC_UART0 -> IIR & 0x01) == 0)
	{
		EspRx.str[EspRx.counter] = LPC_UART0 -> RBR;
		UART_SendByte(LPC_UART3,EspRx.str[EspRx.counter]);
		EspRx.counter++;
		
	}

}


void ADC_configuration(void)
{
		/*
	 * Init ADC pin connect
	 * AD0.2 on P0.25
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 25;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 24;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	
	
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 23;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	
	ADC_Init(LPC_ADC, 200000);
	
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_2,ENABLE);
	
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_1,ENABLE);
	
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_0,ENABLE);
	ADC_BurstCmd(LPC_ADC,ENABLE);

}
uint16_t Read_adc_x(void)
{
   	uint16_t adc_value2;
		
	  adc_value2 =ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_2);
	  return adc_value2;
}
uint16_t Read_adc_y(void)
{
   	uint16_t adc_value2;
	
	  adc_value2 =ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_1);
	  return adc_value2;
}
uint16_t Read_adc_z(void)
{
   	uint16_t adc_value2;
		
	  adc_value2 =ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_0);
	  return adc_value2;
}

void uart_configuration(void)
{

	// uart0
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 2;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 3;
	PINSEL_ConfigPin(&PinCfg);


// uart3
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);



	//set parametr uart
	uart.Baud_rate=115200;
	uart.Parity=UART_PARITY_NONE;
	uart.Databits=UART_DATABIT_8;
	uart.Stopbits=UART_STOPBIT_1;

	UART_Init(LPC_UART0,&uart);
	UART_Init(LPC_UART3,&uart);
		
		//set parametr buffer uart
		fifo.FIFO_ResetRxBuf=ENABLE;
		fifo.FIFO_ResetTxBuf=ENABLE;
		fifo.FIFO_DMAMode=DISABLE;
		fifo.FIFO_Level=UART_FIFO_TRGLEV0;

		UART_FIFOConfig(LPC_UART0,&fifo);
		UART_FIFOConfig(LPC_UART3,&fifo);

		UART_IntConfig(LPC_UART0,UART_INTCFG_RBR,ENABLE);
		UART_IntConfig(LPC_UART3,UART_INTCFG_RBR,ENABLE);
		flag_check = 1;

    NVIC_EnableIRQ(UART0_IRQn);
		NVIC_SetPriority(UART0_IRQn, ((0x01<<3)|0x01));
			
		NVIC_DisableIRQ(UART3_IRQn);
			
		UART_TxCmd(LPC_UART0,ENABLE);
		UART_TxCmd(LPC_UART3,ENABLE);

}



void Call_5ms(void)
{

		if(strstr(EspRx.str,Respons[status]) != NULL)
		{			
			GPIO_SetValue(1,(1<<18));
			flag_check=1;
			EspRx.counter = 0;			
			for(q=0;q<strlen(EspRx.str);q++) {EspRx.str[q] = '0';}
			status ++;
		}
		
		else
		{
			GPIO_ClearValue(1,(1<<18));
		}
		
		
		if(flag_check == 1)
		{
			
			if(status > (StartSendingData+1))
			{
				status = StartSendingData;
			}
			
			else if(status == (StartSendingData+1))
			{
				
				ADCbuffer[ADCSendCounter][ADC_BufferSize - 10] = ADCSendCounter;
				ADCbuffer[ADCSendCounter][ADC_BufferSize -  9] = '\\';
				ADCbuffer[ADCSendCounter][ADC_BufferSize -  8] = '0';
				UART_Send(LPC_UART0,ADCbuffer[ADCSendCounter],sizeof(ADCbuffer[ADCSendCounter]),BLOCKING);
				
				flag_check=0;				
			}
			else
			{
			  UART_Send(LPC_UART0,Command[status],sizeof(Command[status]),BLOCKING);
			  flag_check=0;
				
			}
		}
		
		
		

}



char ESP_Config(void)
{
	  if(strstr(EspRx.str,Respons[status]) != NULL)
		{			
			GPIO_SetValue(1,(1<<18));
			flag_check=1;
			EspRx.counter = 0;			
			for(q=0;q<strlen(EspRx.str);q++) {EspRx.str[q] = '0';}
			status ++;
			
			if (status == (StartSendingData + 1))
			{
				flag_ESP_Config = 1;
				return 1;
			}
			
		}
		
		else
		{
			GPIO_ClearValue(1,(1<<18));
		}
		
		
		if(flag_check == 1)
		{
			 UART_Send(LPC_UART0,Command[status],sizeof(Command[status]),BLOCKING);
			 flag_check=0;
				
		}
		
		flag_ESP_Config = 0;
		return 0;
	
}



















