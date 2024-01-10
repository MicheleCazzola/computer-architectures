#include "button.h"
#include "lpc17xx.h"
#include "../RIT/RIT.h"	
#include "../quoridor/quoridor.h"
#include "../graphics/interface.h"

// Variabili di controllo dei pulsanti
// Inizialmente a 0
// Posti a 1 nei rispettivi interrupt handlers
extern int down_int0;
extern int down_key1;
extern int down_key2;

// Variabili globali esportate
extern MatchType ms;
extern Coordinates WALL_DEFAULT_POS;

// Funzionalità di INT0
// Eseguita una volta per ogni clic del pulsante
// al primo passaggio nel RIT handler
void INT0_function(void){
	
	// Entra in modalità scelta
	setMode(CHOOSING);
	
	// Disegna menu iniziale
	drawMenu(MENU1_Q1, MENU1_Q2, MENU1_OPT1, MENU1_OPT2);
		
	/*
	SEZIONE ESEGUITA DOPO LA CONFERMA COL JOYSTICK
	// Cambio modalità a gioco
	setMode(PLAYING);
	
	// Inizia il giocatore 1
	setPlayer(PLAYER1);
	
	// Abilitazione KEY1: modalità mossa
	enable_button(KEY1_PIN, EINT1_IRQn);
	*/
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
			writeMessage("No walls available, move token");
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


