#include "button.h"
#include "lpc17xx.h"

#include <stdlib.h>
#include "../led/led.h"
#include "../timer/timer.h"
#include "../RIT/RIT.h"	
#include "../quoridor/quoridor.h"

// Control flags
extern int down_int0;
extern int down_key1;
extern int down_key2;

extern MatchType ms;
extern Coordinates WALL_DEFAULT_POS;

void INT0_function(void){
		
	// Cambio modalità a gioco
	setMode(PLAYING);
	
	// Inizia il giocatore 1
	setPlayer(PLAYER1);
	
}

void KEY1_function(void){
		
	// Cancellazione messaggio
	clearMessage();
	
	// Se assenza di muri pendenti
	if(ms.pendingWall == 0){
		
		// Se ancora muri disponibili -> Creazione nuovo muro
		// e abilitazione KEY2: rimane attivo finché è presente
		// un muro non ancora confermato
		if(ms.walls[ms.player-1].used < MAX_NUM_WALLS) {
			newWall(WALL_DEFAULT_POS, HORIZONTAL_WALL);
			//enable_button(12, EINT2_IRQn);
		}
		// Altrimenti -> Messaggio di errore
		else{
			writeMessage("No walls available, move the token");
		}
	}
	// Altrimenti -> Annullamento muro e disabilitazione KEY2
	else{
		//disable_button(12, EINT2_IRQn);
		undoWall();
	}
}

void KEY2_function(void){
	
	// Se in modalità gioco con muri pendenti -> Rotazione del muro
	//if(ms.mode == PLAYING){
		//if(ms.pendingWall == 1){
			rotateWall();
		//}
	//}
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


