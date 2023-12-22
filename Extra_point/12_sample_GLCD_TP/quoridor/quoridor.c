#include "quoridor.h"
#include "../timer/timer.h"
#include "../graphics/interface.h"
#include "../queue/queue.h"

//int mode, time_remaining, /*player,*/ pending_wall;
//int currX[2] = {6, 0};
//int currY[2] = {3, 3};
//int nextPos[2];
//int last_move;
//int validMove;
//wallType walls[2];

const Coordinates START_POS_PLAYER1 = {3,6};
const Coordinates START_POS_PLAYER2 = {3,0};
const Coordinates WALL_DEFAULT_POS = {3,2};

PlayType playState;
Coordinates nextPos;

static int abs(int x){
	return x > 0 ? x : -x;
}

static int posInPlatform(Coordinates pos){
	return 0 <= pos.x && pos.x < NUM_SQUARES && 0 <= pos.y && pos.y < NUM_SQUARES;
}

static int centerWallInPlatform(Coordinates pos){
	return 0 <= pos.x && pos.x < NUM_SQUARES-1 && 0 <= pos.y && pos.y < NUM_SQUARES-1;
}

static int wallBetweenCells(Coordinates srcPos, Coordinates destPos, Coordinates centerPos, int dir){
	//int disconnected = 0;
	if(dir == HORIZONTAL_WALL){
		if(srcPos.x == centerPos.x && destPos.x == centerPos.x && srcPos.y == destPos.y-1 && centerPos.y == srcPos.y) return 1;
		if(srcPos.x == centerPos.x && destPos.x == centerPos.x && destPos.y == srcPos.y-1 && centerPos.y == destPos.y) return 1;
	}
	else{
		if(srcPos.y == centerPos.y && destPos.y == centerPos.y && srcPos.x == destPos.x-1 && centerPos.x == srcPos.x) return 1;
		if(srcPos.y == centerPos.y && destPos.y == centerPos.y && destPos.x == srcPos.x-1 && centerPos.x == destPos.x) return 1;
	}
	return 0;
}

static int noPlayerWallBetween(Coordinates adjPos, int wallPlayer){
	int i;
	for(i = 0; i < playState.walls[wallPlayer-1].used; i++){
		if(wallBetweenCells(playState.currentPos[playState.player-1], adjPos, playState.walls[wallPlayer-1].position[i], playState.walls[wallPlayer-1].dir[i])){
			return 0;
		}
	}
	return 1;
}

static int noWallBetween(Coordinates adjPos){
	return noPlayerWallBetween(adjPos, PLAYER1) && noPlayerWallBetween(adjPos, PLAYER2);
}

static int validPos(Coordinates pos){
	int in_platform, connected;
	
	in_platform = posInPlatform(pos);
	connected = noWallBetween(pos);
	
	return in_platform && connected;
}

// BFS
static int check_reachability(int player){
	
	int queue_dim, i, j;
	Coordinates queue[10];
	int visited[NUM_SQUARES][NUM_SQUARES];
	Coordinates currElem, adjElem;
	/*
	for(i = 0; i < NUM_SQUARES; i++){
		for(j = 0; j < NUM_SQUARES; j++){
			visited[i][j] = 0;
		}
	}*/
	
	queue_dim = 0;
	currElem = playState.currentPos[player-1];
	enqueue(queue, currElem, &queue_dim);
	
	while(!is_empty(queue, queue_dim)){
		currElem = dequeue(queue, &queue_dim);
		visited[currElem.x][currElem.y] = 1;
		
		if(currElem.y == 6*(player-1)){
			clear_queue(queue, &queue_dim);
			return 1;
		}
		
		for(adjElem.x = currElem.x - 1; adjElem.x <= currElem.x + 1; adjElem.x++){
			for(adjElem.y = currElem.y - 1; adjElem.y <= currElem.y + 1; adjElem.y++){
				//targetPos(currElem, tempPos, &finalPos);
				if(validPos(adjElem)){
					if(adjElem.x == currElem.x ^ adjElem.y == currElem.y){
						if(visited[adjElem.x][adjElem.y] == 0){
							enqueue(queue, adjElem, &queue_dim);
						}
					}
				}
			}
		}
	}
	
	return 0;
}

static int exists_overlapping_wall(int playerWalls, Coordinates centerPos, int dir, int *overlapping){
	int i, numWalls, overlap = 0;
	numWalls = playState.walls[playerWalls-1].used;
	for(i = 0; i < numWalls && !overlap; i++){
		if(dir == playState.walls[playerWalls-1].dir[i]){
			*overlapping = i;
			overlap = playState.walls[playerWalls-1].position[i].x == centerPos.x || playState.walls[playerWalls-1].position[i].y == centerPos.y;
		}
		else{
			*overlapping = i;
			overlap = playState.walls[playerWalls-1].position[i].x == centerPos.x && playState.walls[playerWalls-1].position[i].y == centerPos.y;
		}
	} 
	
	return overlap;
}

static int check_not_overlapping(Coordinates centerPos, int dir, int *overlapping, int *player){
	int i, numWalls_p1, numWalls_p2, overlap1, overlap2;
	int overlapping1, overlapping2;
	overlap1 = exists_overlapping_wall(PLAYER1, centerPos, dir, &overlapping1);
	overlap2 = exists_overlapping_wall(PLAYER2, centerPos, dir, &overlapping2);
	
	if(overlap1 || overlap2){
		*player = overlap2;
		*overlapping = (overlap1 == 1) ? overlapping1 : overlapping2;
	}
	
	return !(overlap1 || overlap2);
}

static int validWallPos(Coordinates centerPos, int dir, int *overlapped, int *player){
	int r1, r2, not_overlap, valid_position;
	valid_position = (0 <= centerPos.x && centerPos.x < NUM_SQUARES-1 && 0 <= centerPos.y && centerPos.y < NUM_SQUARES-1);
	r1 = check_reachability(PLAYER1);
	r2 = check_reachability(PLAYER2);
	not_overlap = check_not_overlapping(centerPos, dir, overlapped, player);
	
	return valid_position && r1 && r2 && not_overlap;
}

static int targetPos(Coordinates srcPos, Coordinates destPos, Coordinates *finalPos){
	int stepX = destPos.x - srcPos.x, stepY = destPos.y - srcPos.y;
	if(playState.currentPos[3 - playState.player - 1].x == destPos.x &&
				playState.currentPos[3 - playState.player - 1].y == destPos.y){
		stepY = (srcPos.x == destPos.x) ? -1*(2 * (destPos.y < srcPos.y) - 1)*(abs(destPos.y - srcPos.y)+1) : 0;
		stepX = (srcPos.y == destPos.y) ? -1*(2 * (destPos.x < srcPos.x) - 1)*(abs(destPos.x - srcPos.x)+1) : 0;
	}
	finalPos->x = srcPos.x + stepX;
	finalPos->y = srcPos.y + stepY;
}

static void setWall(Coordinates centerPos, int direction){
	int i = playState.walls[playState.player-1].used;
	playState.walls[playState.player-1].position[i] = centerPos;
	playState.walls[playState.player-1].dir[i] = direction;
	//walls[player-1].used++;
}

static void redrawWalls(){
	int i, j;
	for(i = 0; i < 2; i++){
		for(j = 0; j < playState.walls[i].used; j++){
			drawWall(playState.walls[i].position[j], playState.walls[i].dir[j], WALL_COLOR);
		}
	}
}

static Coordinates getMovedPos(Coordinates currentPos, int h, int v){
	Coordinates newPos;
	newPos.x = currentPos.x + h;
	newPos.y = currentPos.y + v;
	
	return newPos;
}

static void moveWall(int h, int v){
	Coordinates newPos;
	int newDir, i, j, p;
	newPos = playState.walls[playState.player - 1].position[playState.walls[playState.player-1].used];
	newDir = playState.walls[playState.player - 1].dir[playState.walls[playState.player-1].used];
	drawWall(newPos, newDir, BGCOLOR);
	
	redrawWalls();
	
	drawWall(getMovedPos(newPos, h, v), newDir, WALL_COLOR);
	setWall(getMovedPos(newPos, h, v), newDir);
}

void saveMove(int playerId, int moveType, int wallOrientation, Coordinates destPos){
	int newMove = 0;
	newMove |= (playerId << 24);
	newMove |= (moveType << 20);
	newMove |= (wallOrientation << 16);
	newMove |= (destPos.y << 8);
	newMove |= (destPos.x << 0);
	
	playState.last_move = newMove;
}

static void init_players(){
	playState.currentPos[PLAYER1-1] = START_POS_PLAYER1;
	playState.currentPos[PLAYER2-1] = START_POS_PLAYER2;
}

void initGame(){
	LCD_Clear(Blue);
	
	drawChessPlatform();
	
	init_players();
	drawToken(START_POS_PLAYER1, PLAYER1_COLOR);
	drawToken(START_POS_PLAYER2, PLAYER2_COLOR);
	
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
	setColorMove(playState.currentPos[playState.player-1], VALID_MOVE_COLOR);
	enable_timer(0);
}

void setColorMove(Coordinates pos, int color){
	Coordinates tempPos, finalPos;
	
	for(tempPos.x = pos.x - 1; tempPos.x <= pos.x + 1; tempPos.x++){
		for(tempPos.y = pos.y - 1; tempPos.y <= pos.y + 1; tempPos.y++){
			targetPos(pos, tempPos, &finalPos);
			if(validPos(finalPos)){
				if(finalPos.x == pos.x ^ finalPos.y == pos.y){
					drawSquareArea(finalPos, color);
				}
			}
		}
	}
}

void setNextPos(int h, int v){
	Coordinates finalPos;
	Coordinates destPos = getMovedPos(playState.currentPos[playState.player - 1], h, v);
	int finalPosX, finalPosY, i;
	targetPos(playState.currentPos[playState.player-1], destPos, &finalPos);
	if((playState.validMove = validPos(finalPos))){
		nextPos = finalPos;
	}
	else{
		nextPos = playState.currentPos[playState.player-1];
	}
}

void move(){
	reset_timer(0);
	
	drawToken(playState.currentPos[playState.player-1], BGCOLOR);
	setColorMove(playState.currentPos[playState.player-1], BGCOLOR);
	drawToken(nextPos, (playState.player == PLAYER1) ? PLAYER1_COLOR : PLAYER2_COLOR);
	
	playState.currentPos[playState.player-1] = nextPos;
	
	if(playState.validMove){
		saveMove(playState.player-1, PLAYER_MOVE, PLAYER_MOVE, playState.currentPos[playState.player-1]);
	}
	
	setPlayer(3 - playState.player);
}

void newWall(Coordinates centerPos, int direction){
	playState.pending_wall = 1;
	setWall(centerPos, direction);
	//drawWall(rowCenter, colCenter, direction, WALL_COLOR);
	drawWall(centerPos, direction, playState.player == PLAYER1 ? Red : Yellow);
	setColorMove(playState.currentPos[playState.player-1], BGCOLOR);
}

void rotateWall(){
	Coordinates newPos;
	int rowC, colC, currDir, nextDir, i, j;
	newPos = playState.walls[playState.player - 1].position[playState.walls[playState.player-1].used];
	currDir = playState.walls[playState.player - 1].dir[playState.walls[playState.player-1].used];
	nextDir = 1 - currDir;
	drawWall(newPos, currDir, BGCOLOR);
	
	redrawWalls();
	
	drawWall(newPos, nextDir, WALL_COLOR);
	setWall(newPos, nextDir);
}

void confirmWall(){
	int i = playState.walls[playState.player-1].used;
	int overlapped_wall_index, player_index;
	if(validWallPos(playState.walls[playState.player - 1].position[i], playState.walls[playState.player-1].dir[i], &overlapped_wall_index, &player_index)){
		playState.pending_wall = 0;
		playState.walls[playState.player-1].used++;
		saveMove(playState.player-1, WALL_PLACEMENT, playState.walls[playState.player-1].dir[i], playState.walls[playState.player - 1].position[i]);
		writeWalls(MAX_NUM_WALLS - playState.walls[PLAYER1-1].used ,MAX_NUM_WALLS - playState.walls[PLAYER2-1].used);
		setPlayer(3 - playState.player);
	}
	else{
		drawWall(playState.walls[playState.player-1].position[i], playState.walls[playState.player-1].dir[i], BGCOLOR);
		drawWall(playState.walls[player_index].position[overlapped_wall_index], playState.walls[player_index].dir[overlapped_wall_index], WALL_COLOR);
	}
}

void undoWall(){
	int dir;
	Coordinates pos;
	playState.pending_wall = 0;
	pos = playState.walls[playState.player-1].position[playState.walls[playState.player-1].used];
	dir = playState.walls[playState.player-1].dir[playState.walls[playState.player-1].used];
	drawWall(pos, dir, BGCOLOR);
	redrawWalls();
	setColorMove(playState.currentPos[playState.player-1], VALID_MOVE_COLOR);
}

void setNextWall(int h, int v){
	int rowC, colC, i, dir;
	Coordinates pos;
	i = playState.walls[playState.player-1].used;
	pos = getMovedPos(playState.walls[playState.player-1].position[i], h, v);
	dir = playState.walls[playState.player-1].dir[i];
	if(centerWallInPlatform(pos)){
		moveWall(h, v);
	}
}


