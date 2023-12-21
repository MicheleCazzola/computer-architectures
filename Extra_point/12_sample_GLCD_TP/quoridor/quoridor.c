#include "quoridor.h"
#include "../timer/timer.h"
#include "../graphics/interface.h"

int mode, time_remaining, player, pending_wall;
int currX[2] = {6, 0};
int currY[2] = {3, 3};
int nextPos[2];
int last_move;
int validMove;
wallType walls[2];

PlayType playState;

static int abs(int x){
	return x > 0 ? x : -x;
}

static int posInPlatform(int r, int c){
	return 0 <= r && r < NUM_SQUARES && 0 <= c && c < NUM_SQUARES;
}

static int wallBetweenCells(int srcX, int destX, int srcY, int destY, int centerX, int centerY, int dir){
	//int disconnected = 0;
	if(dir == HORIZONTAL_WALL){
		if(srcX == centerX && destX == centerX && srcY == destY+1) return 1;
		if(srcX == centerX && destX == centerX && srcY == destY-1) return 1;
	}
	else{
		if(srcY == centerY && destY == centerY && srcX == destX+1) return 1;
		if(srcY == centerY && destY == centerY && srcX == destX-1) return 1;
	}
	return 0;
}

static int noPlayerWallBetween(int r, int c, int wallPlayer){
	int i;
	for(i = 0; i < walls[wallPlayer-1].used; i++){
		if(wallBetweenCells(currY[player-1], c, currX[player-1], r, walls[wallPlayer-1].rowC[i], walls[wallPlayer-1].colC[i], walls[wallPlayer-1].dir[i])){
			return 0;
		}
	}
	return 1;
}

static int noWallBetween(int r, int c){
	return noPlayerWallBetween(r, c, PLAYER1) && noPlayerWallBetween(r, c, PLAYER2);
}

static int validPos(int r, int c){
	int in_platform, connected;
	
	in_platform = posInPlatform(r,c);
	connected = noWallBetween(r,c);
	
	return in_platform && connected;
}

static int check_reachability(int player){
	return 1;
}

static int exists_overlapping_wall(int playerWalls, int r, int c, int dir, int *overlapping){
	int i, numWalls, overlap = 0;
	numWalls = walls[playerWalls-1].used;
	for(i = 0; i < numWalls && !overlap; i++){
		if(dir == walls[playerWalls-1].dir[i]){
			*overlapping = i;
			overlap = walls[playerWalls-1].rowC[i] == r || walls[playerWalls-1].colC[i] == c;
		}
		else{
			*overlapping = i;
			overlap = walls[playerWalls-1].rowC[i] == r && walls[playerWalls-1].colC[i] == c;
		}
	} 
	
	return overlap;
}

static int check_not_overlapping(int r, int c, int dir, int *overlapping, int *player){
	int i, numWalls_p1, numWalls_p2, overlap1, overlap2;
	int overlapping1, overlapping2;
	overlap1 = exists_overlapping_wall(PLAYER1, r, c, dir, &overlapping1);
	overlap2 = exists_overlapping_wall(PLAYER2, r, c, dir, &overlapping2);
	
	if(overlap1 || overlap2){
		*player = overlap2;
		*overlapping = (overlap1 == 1) ? overlapping1 : overlapping2;
	}
	
	return !(overlap1 || overlap2);
}

static int validWallPos(int r, int c, int dir, int *overlapped, int *player){
	int r1, r2, not_overlap, valid_position;
	valid_position = (0 <= r && r < NUM_SQUARES-1 && 0 <= c && c < NUM_SQUARES-1);
	r1 = check_reachability(PLAYER1);
	r2 = check_reachability(PLAYER2);
	not_overlap = check_not_overlapping(r, c, dir, overlapped, player);
	
	return valid_position && r1 && r2 && not_overlap;
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
	
	playState.last_move = newMove;
}

void initGame(){
	LCD_Clear(Blue);
	
	drawChessPlatform();
	
	drawToken(START_X_PLAYER1, START_Y_PLAYER1, PLAYER2_COLOR);
	drawToken(START_X_PLAYER2, START_Y_PLAYER2, PLAYER1_COLOR);
	
	writeWalls(MAX_NUM_WALLS, MAX_NUM_WALLS);
	setMode(WAITING);
	playState.pending_wall = 0;
}

void setMode(int modeValue){
	playState.mode = modeValue;
	
	if(playState.mode == PLAYING){
		playState.time_remaining = 20;
	}
}

void setPlayer(int playerValue){
	
	playState.player = playerValue;
	playState.time_remaining = 20;
	setColorMove(playState.currentPos[playState.player-1].x, playState.currentPos[playState.player-1].y, VALID_MOVE_COLOR);
	enable_timer(0);
}

void setColorMove(int x, int y, int color){
	int i, j, ri, rj;
	
	for(i = x-1; i <= x+1; i++){
		for(j = y-1; j <= y+1; j++){
			targetPos(x, i, y, j, &ri, &rj);
			if(validPos(ri,rj)){
				if(ri == x ^ rj == y){
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
	//drawWall(rowCenter, colCenter, direction, WALL_COLOR);
	drawWall(rowCenter, colCenter, direction, player == PLAYER1 ? Red : Yellow);
	setColorMove(currX[player-1], currY[player-1], BGCOLOR);
}

void moveWall(int h, int v){
	int rowC, colC, dir, i, j;
	rowC = walls[player-1].rowC[walls[player-1].used];
	colC = walls[player-1].colC[walls[player-1].used];
	dir = walls[player-1].dir[walls[player-1].used];
	drawWall(rowC, colC, dir, BGCOLOR);
	
	for(i = 0; i < 2; i++){
		for(j = 0; j < walls[i].used; j++){
			drawWall(walls[i].rowC[j], walls[i].colC[j], walls[i].dir[j], WALL_COLOR);
		}
	}
	
	drawWall(rowC+h, colC+v, dir, WALL_COLOR);
	setWall(rowC+h, colC+v, dir);
}
	
void rotateWall(){
	int rowC, colC, currDir, nextDir, i, j;
	rowC = walls[player-1].rowC[walls[player-1].used];
	colC = walls[player-1].colC[walls[player-1].used];
	currDir = walls[player-1].dir[walls[player-1].used];
	nextDir = 1 - currDir;
	drawWall(rowC, colC, currDir, BGCOLOR);
	
	for(i = 0; i < 2; i++){
		for(j = 0; j < walls[i].used; j++){
			drawWall(walls[i].rowC[j], walls[i].colC[j], walls[i].dir[j], WALL_COLOR);
		}
	}
	
	drawWall(rowC, colC, nextDir, WALL_COLOR);
	
	setWall(rowC, colC, nextDir);
}

void confirmWall(){
	int i = walls[player-1].used;
	int overlapped_wall_index, player_index;
	if(validWallPos(walls[player-1].rowC[i], walls[player-1].colC[i], walls[player-1].dir[i], &overlapped_wall_index, &player_index)){
		pending_wall = 0;
		walls[player-1].used++;
		saveMove(player-1, WALL_PLACEMENT, walls[player-1].dir[i], walls[player-1].rowC[i], walls[player-1].colC[i]);
		writeWalls(MAX_NUM_WALLS - walls[PLAYER1-1].used ,MAX_NUM_WALLS - walls[PLAYER2-1].used);
		setPlayer(3 - player);
	}
	else{
		drawWall(walls[player-1].rowC[i], walls[player-1].colC[i], walls[player-1].dir[i], BGCOLOR);
		drawWall(walls[player_index].rowC[overlapped_wall_index], walls[player_index].colC[overlapped_wall_index], walls[player_index].dir[overlapped_wall_index], WALL_COLOR);
	}
}

void undoWall(){
	int rowC, colC, dir;
	pending_wall = 0;
	rowC = walls[player-1].rowC[walls[player-1].used];
	colC = walls[player-1].colC[walls[player-1].used];
	dir = walls[player-1].dir[walls[player-1].used];
	drawWall(rowC, colC, dir, BGCOLOR);
	setColorMove(currX[player-1], currY[player-1], VALID_MOVE_COLOR);
}

void setNextWall(int h, int v){
	int rowC, colC, i, dir;
	i = walls[player-1].used;
	rowC = walls[player-1].rowC[i] + h;
	colC = walls[player-1].colC[i] + v;
	dir = walls[player-1].dir[i];
	if(posInPlatform(rowC, colC)){
		moveWall(h, v);
	}
}


