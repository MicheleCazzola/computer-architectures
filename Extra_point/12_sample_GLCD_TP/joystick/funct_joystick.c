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

#define NO_MOVE -1

extern MatchType ms;

/*----------------------------------------------------------------------------
  Function that turns on requested led
 *----------------------------------------------------------------------------*/
void joystick_controller(int *pressed) {
	
	const int moves[5][2] = {
		{NO_MOVE, NO_MOVE}, 	// SELECT
		{0, 1},		// DOWN
		{-1, 0},	// LEFT
		{1, 0},		// RIGHT
		{0, -1}		// UP
	};
	
	int i;	
	for(i = 0; i <= 4; i++){
		// Joystick UP/DOWN/LEFT/RIGHT/SELECT premuto
		if((LPC_GPIO1->FIOPIN & (1<<(i + 25))) == 0){	
			pressed[i]++;
			// Prima pressione
			if(pressed[i] == 1){
				
				disable_button(KEY1_PIN, EINT1_IRQn);
				
				// Modalità movimento pedina
				if(ms.pendingWall == 0){
					// Se SELECT -> Movimento pedina
					// Altrimenti -> Impostazione nuova posizione
					(i == 0) ? move() : setNextPos(moves[i][0], moves[i][1]);
				}
				// Modalità posizionamento muro
				else{
					
					disable_button(KEY2_PIN, EINT2_IRQn);
					
					// Se SELECT -> Conferma muro
					// Altrimenti -> Movimento muro (senza conferma)
					if(i == 0){
						confirmWall();
						
						// Riabilitazione KEY2 solo se ancora in movimento
						if(!ms.validMove){
							enable_button(KEY2_PIN, EINT2_IRQn);
						}
					}
					else{
						setNextWall(moves[i][0], moves[i][1]);
						
						// Riabilitazione KEY2 solo se ancora in movimento
						enable_button(KEY2_PIN, EINT2_IRQn);
					}
				}
				
				// Se la partita non è terminata, si riabilita KEY1
				if(ms.mode == PLAYING){
					enable_button(KEY1_PIN, EINT1_IRQn);
				}
				
				
			}
		}
		// Joystick UP/DOWN/LEFT/RIGHT/SELECT rilasciato
		else{
				pressed[i]=0;
		}
	}
}
