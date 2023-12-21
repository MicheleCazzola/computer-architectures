#include "button.h"
#include "lpc17xx.h"

#include <stdlib.h>
#include "../led/led.h"
#include "../timer/timer.h"
#include "../RIT/RIT.h"	
#include "../quoridor/quoridor.h"

#define VETT_IN_MAX_LEN 100
#define VETT_OUT_MAX_LEN 100
#define CHANGE_SYMBOL 2
#define SPACE 3
#define SENTENCE_END 4

// Control flags
extern int down_int0;
extern int down_key1;
extern int down_key2;	

extern int mode;
extern int pending_wall;

void INT0_function(void){
	if(pending_wall == 0){
		setMode(PLAYING);
		setPlayer(PLAYER1);
	}
}

void KEY1_function(void){
	if(pending_wall == 0){
		newWall(WALL_DEFAULT_COL, WALL_DEFAULT_ROW, HORIZONTAL_WALL);
	}
	else{
		undoWall();
	}
}

void KEY2_function(void){
	if(pending_wall == 1){
		rotateWall();
	}
}
	
void EINT0_IRQHandler (void)	  	/* INT0														 */
{
	down_int0 = 1;
	NVIC_DisableIRQ(EINT0_IRQn);
	LPC_PINCON->PINSEL4 &= ~(1 << 20);
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	down_key1 = 1;
	NVIC_DisableIRQ(EINT1_IRQn);
	LPC_PINCON->PINSEL4 &= ~(1 << 22);
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	down_key2 = 1;
	NVIC_DisableIRQ(EINT2_IRQn);
	LPC_PINCON->PINSEL4 &= ~(1 << 24);
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
}


