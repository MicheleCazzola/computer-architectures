#include "button.h"
#include "lpc17xx.h"

#include <stdlib.h>
#include "../led/led.h"
#include "../timer/timer.h"
#include "../RIT/RIT.h"	

#define VETT_IN_MAX_LEN 100
#define VETT_OUT_MAX_LEN 100
#define CHANGE_SYMBOL 2
#define SPACE 3
#define SENTENCE_END 4

// Morse translation function header
extern int translate_morse(char*, int, char*, int, char, char, char);

// Global variables
unsigned int input_length, output_length;
volatile int res;

// I/O arrays
char vett_input[VETT_IN_MAX_LEN] = {0,0,0,0,2,1,1,1,2,0,1,0,0,2,0,1,3,1,1,2,0,0,1,2,1,0,2,1,0,0,2,1,1,1,3,0,1,1,1,1,2,0,0,1,1,1,4};
char vett_output[VETT_OUT_MAX_LEN];

void EINT0_IRQHandler (void)	  	/* INT0														 */
{
	int i;
	
	//disable_timer(1);
	reset_timer(1);
	LED_Out(0);
	
	for(i = 0; i < output_length; i++){
		vett_output[i] = 0;
	}
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	volatile int i, end, len;
	volatile char c;
	
	// Switch off all leds
	LED_Out(0);
	
	// Read message and compute lengths
	i = 0;
	end = 0;
	len = 0;
	while(!end){
		c = vett_input[i];
		if(c > 1){
			len++;
			if(c == 3)
				len++;
			if(c == 4)
				end = 1;
		}
		i = i+1;
	}
	input_length = i;
	output_length = len;
	//vett_output = malloc(output_length * sizeof(char));
	
	// Switch on all leds
	LED_Out(0xFF);
	
	// Disable INT0 and KEY2
	NVIC_DisableIRQ(EINT0_IRQn);
	NVIC_DisableIRQ(EINT2_IRQn);
	
	// Disable INT0 and KEY2 pins
	LPC_PINCON -> PINSEL4 &= ~(1 << 20);
	LPC_PINCON -> PINSEL4 &= ~(1 << 24);
	
	// Enable timer 0 for 3 s waiting
	enable_timer(0);
	
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	// Disable INT0 and KEY1
	NVIC_DisableIRQ(EINT0_IRQn);
	NVIC_DisableIRQ(EINT1_IRQn);
	
	// Disable INT0 and KEY1 pins
	LPC_PINCON -> PINSEL4 &= ~(1 << 20);
	LPC_PINCON -> PINSEL4 &= ~(1 << 22);
	
	// Morse translation
	res = translate_morse(vett_input, input_length, vett_output, output_length, CHANGE_SYMBOL, SPACE, SENTENCE_END);
	
	// Enable INT0 and KEY1
	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_EnableIRQ(EINT1_IRQn);
	
	// Enable INT0 and KEY1 pins
	LPC_PINCON -> PINSEL4 |= (1 << 20);
	LPC_PINCON -> PINSEL4 |= (1 << 22);
	
	// Enable timer 1 for blinking LEDs
	enable_timer(1);
	
	// Switch on LEDs
	LED_Out(res);
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
}


