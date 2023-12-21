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
#include "../led/led.h"
#include "../quoridor/quoridor.h"


/*----------------------------------------------------------------------------
  Function that turns on requested led
 *----------------------------------------------------------------------------*/
void joystick_On(unsigned int num) {
	
}

int joystick_up(int up){
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		/* Joytick UP pressed */
		up++;
		switch(up){
			case 1: setNextPos(0,-1);
				break;
			default:
				break;
		}
	}
	else{
			up=0;
	}
	
	return up;
}

int joystick_down(int down){
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		/* Joytick DOWN pressed */
		down++;
		switch(down){
			case 1: setNextPos(0,1);
				break;
			default:
				break;
		}
	}
	else{
			down=0;
	}
	
	return down;
}

int joystick_left(int left){
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		/* Joytick LEFT pressed */
		left++;
		switch(left){
			case 1: setNextPos(-1,0);
				break;
			default:
				break;
		}
	}
	else{
			left=0;
	}
	
	return left;
}

int joystick_right(int right){
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick RIGHT pressed */
		right++;
		switch(right){
			case 1: setNextPos(1,0);
				break;
			default:
				break;
		}
	}
	else{
			right=0;
	}
	
	return right;
}

int joystick_select(int sel){
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		/* Joytick SELECT pressed */
		sel++;
		switch(sel){
			case 1: move();
				break;
			default:
				break;
		}
	}
	else{
			sel=0;
	}
	
	return sel;
}
