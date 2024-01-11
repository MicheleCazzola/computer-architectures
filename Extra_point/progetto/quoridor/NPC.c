#include "quoridor.h"
#include "stdlib.h"

#define DOWN 0
#define LEFT_DOWN 1
#define LEFT 2
#define LEFT_UP 3
#define UP 4
#define RIGHT_UP 5
#define RIGHT 6
#define RIGHT_DOWN 7

// Possibili mosse
const Coordinates POSSIBLE_MOVES[8] = {
	{0, 1},								// DOWN
	{-1, 1},							// LEFT-DOWN
	{-1, 0},							// LEFT
	{-1, -1},							// LEFT-UP
	{0, -1},							// UP
	{1, -1},							// RIGHT-UP
	{1, 0},								// RIGHT
	{1, 1}								// RIGHT-DOWN
};

// Check se mossa specifica è tra le selezinate
int moveSelected(Coordinates *selected, char numSel, Coordinates *currentPos, char direction){
	int i;
	
	// Coordinate (x,y) dello spostamento
	Coordinates move = POSSIBLE_MOVES[direction];
	
	for(i = 0; i < numSel; i++){
		
		// Se la destinazione scelta è tra le selezionate, si termina la ricerva
		if(equalCoord(selected[i], changeCoord(*currentPos, move.x, move.y))){
			return 1;
		} 
		
		
	}
	
	return 0;
}


// VERSIONE 1: solo mosse, no muri
void NPC_playTurn(MatchType *status, ModeType *mode, Coordinates *nextPos){
	
	char mainDirection;
	Coordinates currentPos, finalPos, destPos;

	// Posizione corrente del giocatore
	currentPos = status->currentPos[status->player];
	
	// Direzione preferita: verso destinazione
	mainDirection = (status->player == PLAYER1) ? UP : DOWN;
	
	// Selezione celle valide in cui muoversi
	selectAdj(&currentPos, status->player, status->highlited, &(status->numHighlited));
	
	// Scelta cella
	// Mossa ortogonale verso destinazione
	destPos = changeCoord(currentPos, POSSIBLE_MOVES[mainDirection].x, POSSIBLE_MOVES[mainDirection].y);
	if(isHighlitedAdj(&destPos, &finalPos, POSSIBLE_MOVES[mainDirection].x, POSSIBLE_MOVES[mainDirection].y)){
		*nextPos = finalPos;
	}
	else{
		// Mossa diagonale verso destinazione
		if(moveSelected(status->highlited, status->numHighlited, &currentPos, (mainDirection + 1) % 8)){
			*nextPos = changeCoord(currentPos, POSSIBLE_MOVES[(mainDirection + 1) % 8].x,
										POSSIBLE_MOVES[(mainDirection + 1) % 8].y);
		}
		else if(moveSelected(status->highlited, status->numHighlited, &currentPos, (mainDirection - 1 + 8) % 8)){
			*nextPos = changeCoord(currentPos, POSSIBLE_MOVES[(mainDirection - 1 + 8) % 8].x,
										POSSIBLE_MOVES[(mainDirection - 1 + 8) % 8].y);
		}
		// Mossa a caso tra le altre
		else{
			int id;
			
			do{
				id = rand() % 8;
				destPos = changeCoord(currentPos, POSSIBLE_MOVES[id].x, POSSIBLE_MOVES[id].y);
			}while(!isHighlitedAdj(&destPos, &finalPos, POSSIBLE_MOVES[mainDirection].x, POSSIBLE_MOVES[mainDirection].y));
			
			*nextPos = finalPos;
		}
	}
	
	// Set mossa valida
	status->validMove = 1;
	
	// Mossa effettuata
	move();
}
