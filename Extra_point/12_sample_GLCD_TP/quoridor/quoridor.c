#include "quoridor.h"
#include "../timer/timer.h"
#include "../graphics/interface.h"

int mode, time_remaining, player;
int currX[2] = {6, 0}, currY[2] = {3, 3};
int nextPos[2];
int moves[MAX_MOVES_NUMBER];
int movesCounter;
int validMove;

static int abs(int x){
	return x > 0 ? x : -x;
}

static int validPos(int r, int c){
	return 0 <= r && r < NUM_SQUARES && 0 <= c && c < NUM_SQUARES;
}

static int targetPos(int Xsrc, int Xdest, int Ysrc, int Ydest, int *finalPosX, int *finalPosY){
	int stepX = Xdest - Xsrc, stepY = Ydest - Ysrc;
	if(currX[3 - player - 1] == Xdest && currY[3 - player - 1] == Ydest){
		stepY = (Xsrc == Xdest) ? -1*(2 * (Ydest < Ysrc) - 1)*(abs(Ydest - Ysrc)+1) : 0;
		stepX = (Ysrc == Ydest) ? -1*(2 * (Xdest < Xsrc) - 1)*(abs(Xdest - Xsrc)+1) : 0;
	}
	*finalPosX = Xsrc + stepX;
	*finalPosY = Ysrc + stepY;
}

void saveMove(int playerId, int moveType, int wallOrientation, int y, int x){
	int newMove = 0;
	newMove |= (playerId << 24);
	newMove |= (moveType << 20);
	newMove |= (wallOrientation << 16);
	newMove |= (y << 8);
	newMove |= (x << 0);
	
	moves[movesCounter++] = newMove;
}

void initGame(){
	LCD_Clear(Blue);
	
	drawChessPlatform();
	
	drawToken(currX[PLAYER2-1], currY[PLAYER2-1], PLAYER2_COLOR);
	drawToken(currX[PLAYER1-1], currY[PLAYER1-1], PLAYER1_COLOR);
	
	writeWalls(8,8);
	setMode(WAITING);
	movesCounter = 0;
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
	setColorMove(currX[player-1],currY[player-1], VALID_MOVE_COLOR);
	enable_timer(0);
}

void setColorMove(int row, int col, int color){
	int i, j, ri, rj;
	
	for(i = row-1; i <= row+1; i++){
		for(j = col-1; j <= col+1; j++){
			targetPos(row, i, col, j, &ri, &rj);
			if(validPos(ri,rj)){
				if(ri == row ^ rj == col){
					drawSquareArea(ri, rj, color);
				}
			}
		}
	}
}

void setNextPos(int h, int v){
	int finalPosX, finalPosY;
	targetPos(currX[player-1], currX[player-1] + v, currY[player-1], currY[player-1] + h, &finalPosX, &finalPosY);
	if((validMove = validPos(finalPosX, finalPosY))){
		nextPos[0] = finalPosX;
		nextPos[1] = finalPosY;
	}
	else{
		nextPos[0] = currX[player-1];
		nextPos[1] = currY[player-1];
	}
}

void move(){
	reset_timer(0);
	
	drawToken(currX[player-1], currY[player-1], BGCOLOR);
	setColorMove(currX[player-1], currY[player-1], BGCOLOR);
	drawToken(nextPos[0], nextPos[1], (player == PLAYER1) ? PLAYER1_COLOR : PLAYER2_COLOR);
	
	currX[player-1] = nextPos[0];
	currY[player-1] = nextPos[1];
	
	if(validMove){
		// Nella mia logica, X e Y sono invertiti
		saveMove(player-1, PLAYER_MOVE, PLAYER_MOVE, currX[player-1], currY[player-1]);
	}
	
	setPlayer(3 - player);
}


