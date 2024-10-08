/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           joystick.h
** Last modified Date:  2018-12-30
** Last Version:        V1.00
** Descriptions:        Atomic joystick init functions
** Correlated files:    lib_joystick.c, funct_joystick.c
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "joystick.h"

/*----------------------------------------------------------------------------
  Function that initializes joysticks and switch them off
 *----------------------------------------------------------------------------*/

void joystick_init(void) {
	
	// Enable SEL
  LPC_PINCON->PINSEL3 &= ~(3<<18);
	LPC_GPIO1->FIODIR   &= ~(1<<25);
	
	// Enable DOWN
  LPC_PINCON->PINSEL3 &= ~(3<<20);
	LPC_GPIO1->FIODIR   &= ~(1<<26);
	
	// Enable LEFT
  LPC_PINCON->PINSEL3 &= ~(3<<22);
	LPC_GPIO1->FIODIR   &= ~(1<<27);
	
	// Enable RIGHT
  LPC_PINCON->PINSEL3 &= ~(3<<24);
	LPC_GPIO1->FIODIR   &= ~(1<<28);
	
	// Enable UP
  LPC_PINCON->PINSEL3 &= ~(3<<26);
	LPC_GPIO1->FIODIR   &= ~(1<<29);
}
