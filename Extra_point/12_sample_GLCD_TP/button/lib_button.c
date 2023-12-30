
#include "button.h"

/**
 * @brief  Function that initializes Buttons
 */
void BUTTON_init(void) {

  LPC_PINCON->PINSEL4    |= (1 << 20);		 /* External interrupt 0 pin selection */
  LPC_GPIO2->FIODIR      &= ~(1 << 10);    /* PORT2.10 defined as input          */

  LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
  LPC_GPIO2->FIODIR      &= ~(1 << 11);    /* PORT2.11 defined as input          */
  
  LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
  LPC_GPIO2->FIODIR      &= ~(1 << 12);    /* PORT2.12 defined as input          */

  LPC_SC->EXTMODE = 0x7;

  NVIC_EnableIRQ(EINT2_IRQn);              /* enable irq in nvic                 */
	NVIC_SetPriority(EINT2_IRQn, 3);				 /* priority, the lower the better     */
  NVIC_EnableIRQ(EINT1_IRQn);              /* enable irq in nvic                 */
	NVIC_SetPriority(EINT1_IRQn, 2);				 
  NVIC_EnableIRQ(EINT0_IRQn);              /* enable irq in nvic                 */
	NVIC_SetPriority(EINT0_IRQn, 1);				 /* decreasing priority	from EINT2->0	 */
}

void enable_button(int pin, IRQn_Type IRQn){
	LPC_PINCON->PINSEL4 |= (1 << (2 * pin));
	NVIC_EnableIRQ(IRQn);
}
void disable_button(int pin, IRQn_Type IRQn){
	LPC_PINCON->PINSEL4 &= ~(1 << (2 * pin));
	NVIC_DisableIRQ(IRQn);
}

int disabled_button(int pin){
	return (LPC_PINCON->PINSEL4 & (3 << (2*pin))) >> (2*pin) == 0;
}
