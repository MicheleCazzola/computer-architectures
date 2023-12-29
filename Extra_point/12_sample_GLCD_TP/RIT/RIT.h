/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           RIT.h
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        Prototypes of functions included in the lib_RIT, funct_RIT, IRQ_RIT .c files
** Correlated files:    lib_RIT.c, funct_RIT.c, IRQ_RIT.c
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#ifndef __RIT_H
#define __RIT_H

#include <stdint.h>

#define INT0_MASK 		(1 << 0)
#define KEY1_MASK 		(1 << 1)
#define KEY2_MASK 		(1 << 2)
#define JOYSEL_MASK		(1 << 3)
#define JOYDOWN_MASK 	(1 << 4)
#define JOYLEFT_MASK 	(1 << 5)
#define JOYRIGHT_MASK (1 << 6)
#define JOYUP_MASK 		(1 << 7)


/* init_RIT.c */
extern uint32_t init_RIT( uint32_t RITInterval );
extern void enable_RIT( void );
extern void disable_RIT( void );
extern void reset_RIT( void );
/* IRQ_RIT.c */
extern void RIT_IRQHandler (void);

#endif /* end __RIT_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
