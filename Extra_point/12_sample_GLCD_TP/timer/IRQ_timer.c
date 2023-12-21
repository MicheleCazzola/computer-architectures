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

extern int time_remaining, player;
extern int currX[2], currY[2], nextPos[2];

void TIMER0_IRQHandler (void)
{
	writeTimeRemaining(--time_remaining);
	
	reset_timer(0);
	if(time_remaining == 0){
		setColorMove(currX[player-1], currY[player-1], BGCOLOR);
		saveMove(player-1, PLAYER_MOVE, OUT_OF_TIME_MOVE, nextPos[0], nextPos[1]);
		setPlayer(3-player);
	}
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
