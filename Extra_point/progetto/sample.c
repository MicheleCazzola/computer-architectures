#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "timer/timer.h"
#include "graphics/interface.h"
#include "quoridor/quoridor.h"
#include "RIT/RIT.h"
#include "button/button.h"
#include "joystick/joystick.h"

#define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


int main(void)
{
	// Inizializzazione sistema
  SystemInit();  												
	LCD_Initialization();
	
	//TP_Init();
	//TouchPanel_Calibrate();
	
	// Timer 1 s con 25 MHz
	init_timer(0, 0x017D7840);	
	
	// RIT 50 ms con 100 MHz
	init_RIT(0x004C4B40);
	
	// Inizializzazione joystick: abilitazione pin e set a input
	joystick_init();
	
	// Inizializzazione pulsanti: abilitazione pin, set a input
	// e abilitazione interrupt, con set delle priorità
	BUTTON_init();
	
	// Disegno iniziale scacchiera e setup parametri iniziali
	initGame();
	
	// Avvio RIT
	enable_RIT();
	
	// Power-down	mode
	LPC_SC->PCON |= 0x1;									
	LPC_SC->PCON &= ~(0x2);						
	
  while (1)	
  {
		__ASM("wfi");
		
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

