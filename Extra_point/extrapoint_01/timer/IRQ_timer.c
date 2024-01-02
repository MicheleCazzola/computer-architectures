/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../button/button.h"
#include "../TouchPanel/TouchPanel.h"
#include "../quoridor/quoridor.h"
#include "../graphics/interface.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

extern MatchType ms;
extern Coordinates nextPos;

void TIMER0_IRQHandler (void) {
	
	// Stop timer
	reset_timer(0);
	
	// Scrittura tempo rimanente
	writeTimeRemaining(--ms.timeRemaining, TIME_COLOR);
	
	// Tempo per giocatore terminato
	if(ms.timeRemaining == 0){
		
		disable_button(KEY1_PIN, EINT1_IRQn);
		
		// Modalità movimento pedina
		// cancellazione posizioni valide adiacenti
		// salvataggio mossa (out of time move)
		if(ms.pendingWall == 0){
			setColorMove(ms.currentPos[ms.player - 1], BGCOLOR);
			saveMove(ms.player-1, PLAYER_MOVE, OUT_OF_TIME_MOVE, &nextPos);
		}
		// Modalità posizionamento muro: cancellazione muro
		else{
			
			// Disabilitazione KEY2 -> Si passa in modalità movimento pedina
			disable_button(KEY2_PIN, EINT2_IRQn);
			
			drawWall(ms.walls[ms.player-1].position[ms.walls[ms.player-1].used],
									ms.walls[ms.player-1].dir[ms.walls[ms.player-1].used], BGCOLOR);
		}
		
		// Cambio giocatore
		setPlayer(3 - ms.player);
		
		enable_button(KEY1_PIN, EINT1_IRQn);
	}
	// Tempo per giocatore non terminato
	else{
		enable_timer(0);
	}
	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
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
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
