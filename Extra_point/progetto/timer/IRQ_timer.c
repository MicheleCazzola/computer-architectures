#include <string.h>
#include "timer.h"
#include "../button/button.h"
#include "../quoridor/quoridor.h"
#include "../graphics/interface.h"

extern MatchType ms;
extern Coordinates nextPos;

// Timer 0: scatta una volta al secondo, per modificare il tempo rimanente
// Dopo 20 s, effettua il cambio di giocatore
// Priorità 0:
// -	pari a RIT:
// -	superiore ai pulsanti
void TIMER0_IRQHandler (void) {
	
	Coordinates p;
	
	// Scrittura tempo rimanente
	writeTimeRemaining(--ms.timeRemaining, TIME_COLOR);
	
	// Tempo per giocatore terminato
	if(ms.timeRemaining == 0){
		
		// Disabilitazione KEY1
		disable_button(KEY1_PIN, EINT1_IRQn);
		
		// Salvataggio mossa (out of time move)
		saveMove(ms.player, PLAYER_MOVE, OUT_OF_TIME_MOVE, &nextPos);
		
		// Modalità movimento pedina
		if(ms.pendingWall == 0){
			
			// Cancellazione posizioni evidenziate ed eventuale mossa
			eraseHighlightedAdj();
		}
		// Modalità posizionamento muro
		else{
			
			// Disabilitazione KEY2 -> Si passa in modalità movimento pedina
			disable_button(KEY2_PIN, EINT2_IRQn);
			
			// Cancellazione muro
			p = ms.walls[ms.player].position[ms.walls[ms.player].used];
			drawWall(p.x, p.y, ms.walls[ms.player].dir[ms.walls[ms.player].used], BGCOLOR);
		}
		
		// Cambio giocatore
		setPlayer(getOtherPlayer(ms.player));
		
		// Riabilitazione KEY1
		enable_button(KEY1_PIN, EINT1_IRQn);
	}
	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER1_IRQHandler (void) {
	writeMessage("Single board only");
	disable_timer(1);
	
	LPC_TIM1->IR = 1;			/* clear interrupt flag */
}

/******************************************************************************
**                            End Of File
******************************************************************************/
