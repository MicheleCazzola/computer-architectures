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
#include "../joystick/joystick.h"
#include "../timer/timer.h"
#include "../button/button.h"
#include "../quoridor/quoridor.h"

extern void INT0_function(void);
extern void KEY1_function(void);
extern void KEY2_function(void);

extern MatchType ms;

int down_int0 = 0;
int down_key1 = 0;
int down_key2 = 0;
char selected = 0;

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/


void RIT_IRQHandler (void)
{
	
	// Stop RIT: periodo breve, si evita
	// di farlo avanzare durante il suo handler
	disable_RIT();
	reset_RIT();
	
	// INT0: sempre in mutua esclusione rispetto a KEY1 e KEY2
	// Il polling del joystick si basa sullo stato di INT0
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
			
			// Viene riabilitato solo se non ha svolto 
			// INT0_function(), ovvero se KEY1 è ancora disabilitato
			if(disabled_button(KEY1_PIN)){
				enable_button(INT0_PIN, EINT0_IRQn);
			}
		}
	}
	
	// KEY1: abilitato in mutua esclusione a INT0
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
			enable_button(KEY1_PIN, EINT1_IRQn);
		}
	}
	
	// KEY2: solo se non è in corso la gestione
	// dell'interrupt di KEY1 (pressione + 1 RIT)
	// Abilitato in mutua esclusione a INT0
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
			enable_button(KEY2_PIN, EINT2_IRQn);
		}
	}
	
	// Polling joystick, solo se né KEY1 né KEY2 sono premuti
	// e se INT0 è disabilitato, ovvero si è entrati in
	// modalità gioco
	else {
		if(disabled_button(INT0_PIN)){
			joystick_controller();
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
