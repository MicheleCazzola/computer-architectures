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
#include "../BUTTON_EXINT/button.h"

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

extern int joystick_up(int up);
extern int joystick_down(int down);
extern int joystick_left(int left);
extern int joystick_right(int right);
extern int joystick_select(int select);

extern void joystick_controller(int *pressed);

int down_int0 = 0;
int down_key1 = 0;
int down_key2 = 0;

void RIT_IRQHandler (void)
{		
	static int directions[5] = {0, 0, 0, 0, 0};
	
	disable_RIT();
	reset_RIT();
	joystick_controller(directions);
	
	/*
	static int up = 0;
	static int down = 0;
	static int right = 0;
	static int left = 0;
	static int sel = 0;
	
	up = joystick_up(up);
	down = joystick_down(down);
	left = joystick_left(left);
	right = joystick_right(right);
	sel = joystick_select(sel);
	
	*/
	
	// INT0
	if(down_int0 > 0){
		down_int0++;
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){
				switch(down_int0){
					case 2: INT0_function(); break;
					default: break;
				}
			}
		else {	/* button released */
			down_int0=0;
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}
	
	// KEY1
	if(down_key1 > 0){
		down_key1++;
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){
				switch(down_key1){
					case 2: KEY1_function(); break;
					default: break;
				}
			}
		else {	/* button released */
			down_key1=0;
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	
	// KEY2
	if(down_key2 > 0){
		down_key2++;
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){
				switch(down_key2){
					case 2: KEY2_function(); break;
					default: break;
				}
			}
		else {	/* button released */
			down_key2=0;
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	}
	
	enable_RIT();
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
