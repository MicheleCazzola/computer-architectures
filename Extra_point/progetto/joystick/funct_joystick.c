/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           funct_joystick.h
** Last modified Date:  2018-12-30
** Last Version:        V1.00
** Descriptions:        High level joystick management functions
** Correlated files:    lib_joystick.c, funct_joystick.c
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "joystick.h"
#include "../button/button.h"
#include "../quoridor/quoridor.h"
#include "../coordinates/coordinates.h"

#define NO_MOVE -1

// Variabile di stato esportata
extern MatchType ms;
extern Coordinates nextPos;

// Conta numero di input del joystick selezionati
static int countPressed(){
	int i, r;
	for(i = r = 0; i < 5; i++){
		r += ((LPC_GPIO1->FIOPIN & (1 << (i + 25))) == 0);
	}
	
	return r;
}

// Funzione di controllo del joystick
void joystick_controller() {
	
	// Mosse possibili
	const int moves[5][2] = {
		{NO_MOVE, NO_MOVE}, 	// SELECT
		{0, 1},								// DOWN
		{-1, 0},							// LEFT
		{1, 0},								// RIGHT
		{0, -1}								// UP
	};
	
	// Vettore di contatori per ogni possibile direzione di input
	// Incrementato ogni volta che l'input corrispondente risulta premuto
	static int pressed[5] = {0, 0, 0, 0, 0};
	
	int i, h, v, sel;
	
	// Direzioni diagonali non consentite per spostamento muri
	if(countPressed() > 1 && ms.pendingWall == 1){
		return;
	}
	
	
	
	h = v = sel = 0;
	for(i = 0; i <= 4; i++){
		// Joystick UP/DOWN/LEFT/RIGHT/SELECT premuto
		if((LPC_GPIO1->FIOPIN & (1 << (i + 25))) == 0){
			pressed[i]++;
			
			// Se prima pressione, calcolo incrementi h/v
			if(pressed[i] == 1){
				if(i == 0){
					sel = 1;
				}
				else{
					h += moves[i][0];
					v += moves[i][1];
				}
				
			}
		}
		// Joystick UP/DOWN/LEFT/RIGHT/SELECT rilasciato
		else{
				pressed[i]=0;
		}
	}
	
	// Se almeno un comando � premuto, si svolgono le funzioni del joystick
	if(countPressed() > 0){
		
		// Disabilitazione KEY1 durante lo svolgimento
		// delle funzioni del joystick
		disable_button(KEY1_PIN, EINT1_IRQn);
		
		// Modalit� movimento pedina
		if(ms.pendingWall == 0){
			
			// Se SELECT -> Movimento pedina
			// Altrimenti -> Impostazione nuova posizione
			if(sel){
				
				// Se la posizione di arrivo � diversa da quella corrente -> Spostamento
				// Si impedisce di confermare la non-mossa della pedina
				if(!equalCoord(nextPos, ms.currentPos[ms.player])){
					move();
				}
			}
			else{
				setNextPos(h ,v);
			}
		}
		// Modalit� posizionamento muro
		else{
			
			// Disabilitazione KEY1 durante lo svolgimento
			// delle funzioni del joystick, solo se era
			// precedentemente abilitato (modalit� muro)
			disable_button(KEY2_PIN, EINT2_IRQn);
			
			// Se SELECT -> Conferma muro
			// Altrimenti -> Movimento muro (senza conferma)
			if(sel){
				confirmWall();
				
				// Riabilitazione KEY2 solo se ancora in movimento,
				// a causa di conferma fallita (posizione non valida)
				// Si usa come condizione essere ancora in modalit� muro
				if(ms.pendingWall == 1){
					enable_button(KEY2_PIN, EINT2_IRQn);
				}
			}
			else{
				setNextWall(h, v);
				
				// Riabilitazione KEY2 solo se ancora in movimento,
				// a causa di input diverso da conferma del muro
				enable_button(KEY2_PIN, EINT2_IRQn);
			}
		}
		
		// Se la partita non � terminata, si riabilita KEY1
		if(ms.mode == PLAYING){
			enable_button(KEY1_PIN, EINT1_IRQn);
		}
	}
	
	
}
