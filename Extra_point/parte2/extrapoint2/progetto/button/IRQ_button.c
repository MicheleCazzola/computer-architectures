#include "button.h"
#include "lpc17xx.h"
#include "../RIT/RIT.h"	
#include "../quoridor/quoridor.h"
#include "../graphics/interface.h"
#include "../CAN/CAN.h"
#include "../timer/timer.h"

// Variabili di controllo dei pulsanti
// Inizialmente a 0
// Posti a 1 nei rispettivi interrupt handlers
extern int down_int0;
extern int down_key1;
extern int down_key2;

// Variabili globali esportate
extern ModeType gm;
extern MatchType ms;
extern Coordinates WALL_DEFAULT_POS;
extern const int PLAYER_COLORS[2];
extern CAN_msg CAN_TxMsg;

// Funzionalità di INT0
// Eseguita una volta per ogni clic del pulsante
// al primo passaggio nel RIT handler
void INT0_function(void){
	
	unsigned char dataVett[2] = {HANDSHAKE_PREFIX, 0};
	
	// Entra in modalità scelta
	setMode(CHOOSING);
	
	// Disegna menu iniziale
	drawMenu(MENU1_Q1, MENU1_Q2, MENU1_OPT1, MENU1_OPT2);
	
	// Scheda del primo giocatore
	if(gm.handshake == HANDSHAKE_OFF){
		
		// Giocatore 1
		gm.boardPlayer = PLAYER1;
		
		// Handshake iniziato
		gm.handshake = dataVett[1] = HANDSHAKE_ON;
		
		// Avvio timer per esistenza avversario
		enable_timer(1);
	}
	// Scheda del secondo giocatore
	else if(gm.handshake == HANDSHAKE_ON){
		
		// Giocatore 2
		gm.boardPlayer =  PLAYER2;
		
		// Handshake iniziale completato:
		// le schede conoscono a vicenda la loro esistenza
		gm.handshake = dataVett[1] = HANDSHAKE_DONE;
	}

	// Scrittura messaggio di handshake su CAN
	CAN_buildMsg(1, dataVett, 2, STANDARD_FORMAT, DATA_FRAME);	
	CAN_wrMsg(1, &CAN_TxMsg);
}

// Funzionalità di KEY1
// Eseguita una volta per ogni clic del pulsante
// al primo passaggio nel RIT handler
void KEY1_function(void){
		
	// Cancellazione messaggio
	clearMessage();
	
	// Se assenza di muri pendenti
	if(ms.pendingWall == 0){
		
		// Se ancora muri disponibili -> 
		// -	Creazione nuovo muro
		// -	Abilitazione KEY2: rimane attivo finché è presente
		// 		un muro non ancora confermato
		if(ms.walls[ms.player].used < MAX_NUM_WALLS) {
			newWall(WALL_DEFAULT_POS, HORIZONTAL_WALL);
			enable_button(KEY2_PIN, EINT2_IRQn);
		}
		// Altrimenti -> Messaggio di errore
		else{
			writeMessage("No walls available, move token", XPOS_MESSAGE, YPOS_MESSAGE, MESSAGE_COLOR, BGCOLOR);
		}
	}
	// Altrimenti -> 
	// - 	Disabilitazione KEY2 (si passa a modalità mossa)
	// -	Annullamento muro
	else{
		disable_button(KEY2_PIN, EINT2_IRQn);
		undoWall();
	}
}

// Funzionalità di KEY2
// Eseguita una volta per ogni clic del pulsante
// al primo passaggio nel RIT handler
void KEY2_function(void){
	
	// Disabilitazione KEY1 durante la rotazione
	disable_button(KEY1_PIN, EINT1_IRQn);
	
	// Rotazione muro
	rotateWall();
	
	// Riabilitazione KEY1
	enable_button(KEY1_PIN, EINT1_IRQn);
}

// INT0 interrupt handler
void EINT0_IRQHandler (void)
{
	down_int0 = 1;
	disable_button(INT0_PIN, EINT0_IRQn);
	
	// Clear pending interrupt
	LPC_SC->EXTINT &= (1 << 0);     
}

// KEY1 interrupt handler
void EINT1_IRQHandler (void)
{
	down_key1 = 1;
	disable_button(KEY1_PIN, EINT1_IRQn);
	
	// Clear pending interrupt
	LPC_SC->EXTINT &= (1 << 1);
}

// KEY2 interrupt handler
void EINT2_IRQHandler (void)
{
	down_key2 = 1;
	disable_button(KEY2_PIN, EINT2_IRQn);
	
	// Clear pending interrupt
  LPC_SC->EXTINT &= (1 << 2);   
}


