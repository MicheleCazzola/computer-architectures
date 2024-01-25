#include "timer.h"
#include "../button/button.h"
#include "../quoridor/quoridor.h"
#include "../graphics/interface.h"

extern MatchType ms;
extern ModeType gm;
extern Coordinates nextPos;

// Timer 0: scatta una volta al secondo, per modificare il tempo rimanente
// Dopo 20 s, effettua il cambio di giocatore
// Priorità 0:
// -	pari a RIT
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
		
		// Invio mossa
		if(gm.numBoards == 2){
			sendMove();
		}
		
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
		
		// Single-board
		if(gm.numBoards == 1){
			// Cambio giocatore
			setPlayer(getOtherPlayer(ms.player));
			
			// Riabilitazione KEY1
			enable_button(KEY1_PIN, EINT1_IRQn);
		}
		// Double-board
		else{
			
			// Set nuovo giocatore e pulizia scacchiera
			setOpponentTurn();
		}
		
	}
	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}

// Timer per fase 1 handshake
// Priorità 0:
// -	pari a RIT
// -	superiore ai pulsanti
void TIMER1_IRQHandler (void) {
	
	// Timeout: solo una scheda presente, si disabilita e resetta il timer
	writeMessage("Single board only", XPOS_OPPONENT_MESSAGE, YPOS_OPPONENT_MESSAGE, TEXTCOLOR_MENU, BGCOLOR_MENU);
	disable_timer(1);
	reset_timer(1);
	
	LPC_TIM1->IR = 1;			/* clear interrupt flag */
}

/******************************************************************************
**                            End Of File
******************************************************************************/
