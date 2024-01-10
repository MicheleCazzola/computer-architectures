/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../joystick/joystick.h"
#include "../button/button.h"
#include "../quoridor/quoridor.h"

// Variabili di controllo dei pulsanti
// Incrementate ad ogni RIT handler, se avviene
// immediatamente in seguito alla pressione del
// pulsante corrispondente
int down_int0 = 0;
int down_key1 = 0;
int down_key2 = 0;

extern MatchType ms;

// RIT handler -> Implementa:
// -	debouncing INT0, KEY1, KEY2
// -	polling joystick
// evitando di esaminare input multipli quando possibile
// Priorità 0:
// -	pari a TIMER0
// -	superiore ai pulsanti
void RIT_IRQHandler (void)
{
	// Stop RIT: periodo breve, si evita
	// di farlo avanzare durante il suo handler
	disable_RIT();
	reset_RIT();
	
	// Polling joystick per scelta modalità di gioco
	// Effettuato solo se scelta in corso
	if(ms.mode == CHOOSING){
		joystick_controller_chooseMode();
	}
	
	// INT0: sempre in mutua esclusione rispetto a KEY1 e KEY2
	// Solo se in modalità di attesa
	if(ms.mode == WAITING){
		if(down_int0 > 0){
			down_int0++;
			if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){
					switch(down_int0){
						case 2: INT0_function(); break;
						default: break;
					}
				}
			else {
				down_int0=0;
				
				// Viene riabilitato solo se non ha svolto 
				// INT0_function(), ovvero se la modalità è ancora WAITING
				if(ms.mode == WAITING){
					enable_button(INT0_PIN, EINT0_IRQn);
				}
			}
		}
	}
	
	// KEY1, KEY2 e il joystick (per movimento pedina/muri)
	// sono verificati solo se in modalità di gioco
	// KEY1: abilitato in mutua esclusione a INT0
	if(ms.mode == PLAYING){
		if(down_key1 > 0){
			down_key1++;
			if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){
					switch(down_key1){
						case 2: KEY1_function(); break;
						default: break;
					}
				}
			else {
				down_key1=0;
				enable_button(KEY1_PIN, EINT1_IRQn);
			}
		}
		
		// KEY2: esaminato solo se non è in corso la gestione
		// dell'interrupt di KEY1 (pressione + un ciclo di RIT)
		// Abilitato in mutua esclusione a INT0
		else if(down_key2 > 0){
			down_key2++;
			if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){
					switch(down_key2){
						case 2: KEY2_function(); break;
						default: break;
					}
				}
			else {
				down_key2=0;
				enable_button(KEY2_PIN, EINT2_IRQn);
			}
		}
		
		// Polling joystick, solo se né KEY1 né KEY2 sono premuti
		// e se INT0 è disabilitato, ovvero si è entrati in
		// modalità gioco
		else {
			if(disabled_button(INT0_PIN)){
				joystick_controller_move();
			}
		}
	}
	
	// Riavvio RIT
	enable_RIT();
	
	// Clear interrupt flag
  LPC_RIT->RICTRL |= 0x1;	
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
