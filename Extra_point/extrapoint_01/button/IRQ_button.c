#include "button.h"
#include "lpc17xx.h"
#include "../RIT/RIT.h"	
#include "../quoridor/quoridor.h"
#include "../graphics/interface.h"

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
	
	enable_button(KEY1_PIN, EINT1_IRQn);
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
			enable_button(KEY2_PIN, EINT2_IRQn);
		}
		// Altrimenti -> Messaggio di errore
		else{
			writeMessage("No walls available, move the token");
		}
	}
	// Altrimenti -> Annullamento muro
	else{
		disable_button(KEY2_PIN, EINT2_IRQn);
		undoWall();
	}
}

void KEY2_function(void){
	
	disable_button(KEY1_PIN, EINT1_IRQn);
	
	// Rotazione muro
	rotateWall();
	
	enable_button(KEY1_PIN, EINT1_IRQn);
}
	
void EINT0_IRQHandler (void)	  	/* INT0														 */
{
	down_int0 = 1;
	disable_button(INT0_PIN, EINT0_IRQn);
	
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	down_key1 = 1;
	disable_button(KEY1_PIN, EINT1_IRQn);
	
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	down_key2 = 1;
	disable_button(KEY2_PIN, EINT2_IRQn);
	
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
}


