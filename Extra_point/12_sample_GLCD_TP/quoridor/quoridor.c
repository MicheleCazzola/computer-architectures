#include "quoridor.h"
#include "../timer/timer.h"
#include "../graphics/interface.h"

int mode, time_remaining, player;
int currX[2] = {6, 0};
int currY[2] = {3, 3};
int nextPos[2];
int last_move;
int validMove;
wallType walls[2];
int pending_wall;

static int abs(int x){
	return x > 0 ? x : -x;
}

static int validPos(int r, int c){
	return 0 <= r && r < NUM_SQUARES && 0 <= c && c < NUM_SQUARES;
}

static int check_reachability(int player){
	return 1;
}

static int exists_overlapping_wall(int playerWalls, int r, int c, int dir){
	int i, numWalls, overlap = 0;
	numWalls = walls[playerWalls-1].used + (playerWalls != player);
	for(i = 0; i < numWalls && !overlap; i++){
		if(dir == walls[playerWalls-1].dir[i]){
			overlap = overlap || (walls[playerWalls-1].rowC[i] == r) || (walls[playerWalls-1].colC[i] == c);
		}
		else{
			overlap = overlap || ((walls[playerWalls-1].rowC[i] == r) && (walls[playerWalls-1].colC[i] == c));
		}
	} 
	
	return overlap;
}

static int check_not_overlapping(int r, int c, int dir){
	int i, numWalls_p1, numWalls_p2, overlap1, overlap2;
	overlap1 = exists_overlapping_wall(PLAYER1, r, c, dir);
	overlap2 = exists_overlapping_wall(PLAYER2, r, c, dir);
	return !(overlap1 || overlap2);
}

static int validWallPos(int r, int c, int dir){
	int r1, r2, not_overlap;
	r1 = check_reachability(PLAYER1);
	r2 = check_reachability(PLAYER2);
	not_overlap = check_not_overlapping(r, c, dir);
	
	return r1 && r2 && not_overlap;
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

static void setWall(int rowCenter, int colCenter, int direction){
	int i = walls[player-1].used;
	walls[player-1].rowC[i] = rowCenter;
	walls[player-1].colC[i] = colCenter;
	walls[player-1].dir[i] = direction;
	//walls[player-1].used++;
}

void saveMove(int playerId, int moveType, int wallOrientation, int y, int x){
	int newMove = 0;
	newMove |= (playerId << 24);
	newMove |= (moveType << 20);
	newMove |= (wallOrientation << 16);
	newMove |= (y << 8);
	newMove |= (x << 0);
	
	last_move = newMove;
}

void initGame(){
	LCD_Clear(Blue);
	
	drawChessPlatform();
	
	drawToken(currX[PLAYER2-1], currY[PLAYER2-1], PLAYER2_COLOR);
	drawToken(currX[PLAYER1-1], currY[PLAYER1-1], PLAYER1_COLOR);
	
	writeWalls(8,8);
	setMode(WAITING);
	pending_wall = 0;
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
	int finalPosX, finalPosY, i;
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

void newWall(int rowCenter, int colCenter, int direction){
	pending_wall = 1;
	setWall(rowCenter, colCenter, direction);
	drawWall(rowCenter, colCenter, direction, WALL_COLOR);
}

void moveWall(int h, int v){
	int rowC, colC, dir;
	rowC = walls[player-1].rowC[walls[player-1].used];
	colC = walls[player-1].colC[walls[player-1].used];
	dir = walls[player-1].dir[walls[player-1].used];
	drawWall(rowC, colC, dir, BGCOLOR);
	drawWall(rowC+h, colC+v, dir, WALL_COLOR);
	setWall(rowC+h, colC+v, dir);
}
	
void rotateWall(){
	int rowC, colC, currDir, nextDir;
	rowC = walls[player-1].rowC[walls[player-1].used];
	colC = walls[player-1].colC[walls[player-1].used];
	currDir = walls[player-1].dir[walls[player-1].used];
	nextDir = 1 - currDir;
	drawWall(rowC, colC, currDir, BGCOLOR);
	drawWall(rowC, colC, nextDir, WALL_COLOR);
	
	setWall(rowC, colC, nextDir);
}

void confirmWall(){
	pending_wall = 0;
	walls[player-1].used++;
}

void undoWall(){
	int rowC, colC, dir;
	pending_wall = 0;
	rowC = walls[player-1].rowC[walls[player-1].used];
	colC = walls[player-1].colC[walls[player-1].used];
	dir = walls[player-1].dir[walls[player-1].used];
	drawWall(rowC, colC, dir, BGCOLOR);
}

void setNextWall(int h, int v){
	int rowC, colC, i, dir;
	i = walls[player-1].used;
	rowC = walls[player-1].rowC[i] + v;
	colC = walls[player-1].colC[i] + h;
	dir = walls[player-1].dir[i];
	if(validWallPos(rowC, colC, dir)){
		moveWall(h, v);
	}
}


