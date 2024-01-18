/*----------------------------------------------------------------------------
 * Name:    Can.c
 * Purpose: CAN interface for for LPC17xx with MCB1700
 * Note(s): see also http://www.port.de/engl/canprod/sv_req_form.html
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <lpc17xx.h>                  /* LPC17xx definitions */
#include "CAN.h"                      /* LPC17xx CAN adaption layer */
#include "../GLCD/GLCD.h"
#include "../quoridor/quoridor.h"
#include "../graphics/interface.h"
#include "../timer/timer.h"
#include "../button/button.h"

extern ModeType gm;
extern MatchType ms;
extern uint8_t icr ; 										//icr and result must be global in order to work with both real and simulated landtiger.
extern uint32_t result;
extern CAN_msg       CAN_TxMsg;    /* CAN message for sending */
extern CAN_msg       CAN_RxMsg;    /* CAN message for receiving */                                

/*----------------------------------------------------------------------------
  CAN interrupt handler
 *----------------------------------------------------------------------------*/
void CAN_IRQHandler (void)  {
	
	unsigned char moveType, wallDir;
	int x, y;

  /* check CAN controller 1 */
	icr = 0;
  icr = (LPC_CAN1->ICR | icr) & 0xFF;               /* clear interrupts */
	
	/* CAN Controller #1 meassage is received */
  if (icr & (1 << 0)) {    
		
		/* Read the message */
		CAN_rdMsg (1, &CAN_RxMsg);	  

		/* Release receive buffer */
    LPC_CAN1->CMR = (1 << 2);                    		
		
		// Ricezione messaggio di handshake
		if(CAN_RxMsg.data[0] == 0xFF){
			gm.handshake = CAN_RxMsg.data[1];
			
			// Stop timer se handshake iniziale effettuato
			if(gm.handshake == HANDSHAKE_DONE){
				disable_timer(1);
			}
			
			// TEST
			writeMessage("Message arrived");
		}
		// Ricezione mossa avversario -> Salva e gioca
		else{
			
			// Riabilitazione KEY1
			enable_button(KEY1_PIN, EINT1_IRQn);
			
			// Alla prima mossa ricevuta, PLAYER2 va in PLAYING
			if(ms.mode == READY){
				setMode(PLAYING);
			}
			
			// Aggiornamento stato gioco
			moveType = (CAN_RxMsg.data[2] >> 4) & 0xF;
			wallDir = CAN_RxMsg.data[2] & 0xF;
			y = (int)CAN_RxMsg.data[1];
			x = (int)CAN_RxMsg.data[0];
			
			updateOpponentData(CAN_RxMsg.data[3], moveType, wallDir, y, x);
			
			// Aggiornamento statistiche ad ogni ricezione mossa
			writeWallsStats(getAvailableWalls(PLAYER1), getAvailableWalls(PLAYER2));
			
			// Check vittoria
			if(victory(ms.currentPos[getOtherPlayer(gm.boardPlayer)], getOtherPlayer(gm.boardPlayer))){
				setVictoryMessage();
				initGame();
			}
			else{
				// Set nuovo giocatore
				setPlayer(gm.boardPlayer);
			}
		}
  }
}
