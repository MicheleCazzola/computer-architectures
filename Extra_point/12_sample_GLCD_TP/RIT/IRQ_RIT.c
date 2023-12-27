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
#include "../button/button.h"
#include "../quoridor/quoridor.h"

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

extern void joystick_controller(int *pressed);

extern MatchType ms;

int down_int0 = 0;
int down_key1 = 0;
int down_key2 = 0;

void RIT_IRQHandler (void)
{		
	static int directions[5] = {0, 0, 0, 0, 0};
	
	// Stop RIT: periodo breve, si evita
	// di farlo avanzare durante il suo handler
	disable_RIT();
	reset_RIT();
	
	// Polling joystick, solo se KEY1 è abilitata
	// Il joystick è sempre abilitato, ma viene
	// controllato in polling solo se può produrre
	// un movimento della pedina/muro sulla scacchiera
	// Nessun controllo su INT0, in quanto è controllato
	// già dalla prima condizione
	// COME EFFETTUARE:
	// Se uno tra KEY1 e KEY2 è premuto, non ha effetto
	if((LPC_PINCON->PINSEL4 & (3 << 22)) >> 22 == 1){
		joystick_controller(directions);
	}
	
	// INT0: sempre in mutua esclusione rispetto a KEY1,
	// KEY2 e joystick
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
			if((LPC_PINCON->PINSEL4 & (3 << 22)) >> 22 == 0){
				NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
				LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
			}
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
	
	// KEY2: se viene premuto insieme a KEY1, non ha effetto
	else if(down_key2 > 0){
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
	
	// Riavvio RIT
	enable_RIT();
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
