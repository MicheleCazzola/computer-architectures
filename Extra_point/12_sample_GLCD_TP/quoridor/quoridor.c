#include "quoridor.h"
#include "../graphics/interface.h"

int mode, time_remaining, player;
int currX[2] = {0, 6}, currY[2] = {3, 3};

void initGame(){
	
	
	LCD_Clear(Blue);
	
	drawChessPlatform();
	drawToken(0,3,PLAYER2);
	drawToken(6,3,PLAYER1);
	
	writeWalls(8,8);
	setMode(WAITING);
}

void setMode(int modeValue){
	mode = modeValue;
	
	if(mode == PLAYING){
		time_remaining = 20;
	}
}

void setPlayer(int playerValue){
	player = playerValue;
	time_remaining = 20;
	highlightValidMoves(6,3);
}

void highlightValidMoves(int row, int col){
	int i, j;
	
	for(i = row-1; i <= row+1; i++){
		for(j = col-1; j <= col+1; j++){
			if(0 < i && i < NUM_SQUARES && 0 < j && j < NUM_SQUARES){
				if(i == row ^ j == col){
					drawSquareArea(i, j);
				}
			}
		}
	}
}

void move(int h, int v){
	drawToken(currX[player-1]+h, currY[player-1]+v, PLAYER1);
}