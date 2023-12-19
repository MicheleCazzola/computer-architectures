/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../timer/timer.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

extern void INT0_function(void);
extern void KEY1_function(void);
extern void KEY2_function(void);
extern int rit_enabled;

void RIT_IRQHandler (void)
{			
	static int down_int0 = 0;
	static int down_key1 = 0;
	static int down_key2 = 0;
	
	// INT0
	if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){
			down_int0++;
			reset_RIT();
			switch(down_int0){
				case 1: INT0_function(); break;
				default: break;
			}
		}
	else {	/* button released */
		down_int0=0;	
		disable_RIT();
		rit_enabled = 0;
		reset_RIT();
		NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		//triggering &= ~(1 << 0);
	}
	
	// KEY1
	if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){
			down_key1++;
			reset_RIT();
			switch(down_key1){
				case 1: KEY1_function(); break;
				default: break;
			}
		}
	else {	/* button released */
		down_key1=0;			
		disable_RIT();
		rit_enabled = 0;
		reset_RIT();
		NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		//triggering &= ~(1 << 1);
	}
	
	// KEY2
	if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){
			down_key2++;
			reset_RIT();
			switch(down_key2){
				case 1: KEY2_function(); break;
				default: break;
			}
		}
	else {	/* button released */
		down_key2=0;			
		disable_RIT();
		rit_enabled = 0;
		reset_RIT();
		NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		//triggering &= ~(1 << 2);
	}
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
