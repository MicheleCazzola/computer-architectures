/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "timer.h"
#include "../led/led.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern unsigned char led_value;					/* defined in funct_led								*/
extern int res;													/* defined in IRQ_button							*/

void TIMER0_IRQHandler (void)
{
	// Switch off all leds
	LED_Out(0);
	
	// Reset this timer
	reset_timer(0);
	
	// Enable INT0 and KEY2
	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_EnableIRQ(EINT2_IRQn);
	
	// Enable INT0 and KEY2 pins
	LPC_PINCON->PINSEL4 |= (1 << 20);
	LPC_PINCON->PINSEL4 |= (1 << 24);
	
  LPC_TIM0->IR |= 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	// Compute LED value
	if(led_value == 0){
		led_value = res;
	}
	else{
		led_value = 0;
	}
	
	// Switch on/off LEDs according on result value and current state
	LED_Out(led_value);
	
	// Disable and re-enable this timer -> Blinking
	disable_timer(1);
	enable_timer(1);
	
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
