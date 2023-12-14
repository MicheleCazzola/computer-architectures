#include "button.h"
#include "lpc17xx.h"

#include "../led/led.h"
#include "../timer/timer.h"
#include "../RIT/RIT.h"	

extern int translate_morse(char*, int, char*, int, char, char, char);

#define VETT_IN_MAX_LEN 100
#define VETT_OUT_MAX_LEN 100
#define CHANGE_SYMBOL 2
#define SPACE 3
#define SENTENCE_END 4

char vett_input[VETT_IN_MAX_LEN] = {0,0,0,0,2,1,1,1,2,0,1,0,0,2,0,1,3,1,1,2,0,0,1,2,1,0,2,1,0,0,2,1,1,1,3,0,1,1,1,1,2,0,0,1,1,1,4};
char vett_output[VETT_OUT_MAX_LEN];
	
unsigned int input_length, output_length;
volatile int res;

void EINT0_IRQHandler (void)	  	/* INT0														 */
{
	//disable_timer(1);
	reset_timer(1);
	LED_Out(0);
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	int i, end;
	char c;
	
	// Switch off all leds
	LED_Out(0);
	//enable_RIT();
	
	//NVIC_DisableIRQ(EINT1_IRQn);
	
	// Read message and compute lengths
	i = 0;
	end = 0;
	input_length = 0;
	output_length = 0;
	while(!end){
		c = vett_input[i];
		if(c == 2 || c == 3){
			output_length++;
		}
		else if(c == 4){
			end = 1;
		}
		i = i+1;
	}
	input_length = i;
	output_length++;
	
	// Switch on all leds
	LED_Out(0xFF);
	
	// Disable INT0 and KEY2
	NVIC_DisableIRQ(EINT0_IRQn);
	NVIC_DisableIRQ(EINT2_IRQn);
	
	enable_timer(0);
	
	//LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	//enable_RIT();
	NVIC_DisableIRQ(EINT0_IRQn);
	NVIC_DisableIRQ(EINT1_IRQn);
	res = translate_morse(vett_input, input_length, vett_output, output_length, CHANGE_SYMBOL, SPACE, SENTENCE_END);
	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_EnableIRQ(EINT1_IRQn);
	enable_timer(1);
	LED_Out(res);
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
}


