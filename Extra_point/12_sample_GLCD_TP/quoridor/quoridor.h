#ifndef _QUORIDOR_H_
#define _QUORIDOR_H_

#include "../graphics/interface.h"

#define WAITING 0
#define PLAYING 1
#define PLAYER1 1
#define PLAYER2 2
#define PLAYER_MOVE 0
#define WALL_PLACEMENT 1
#define OUT_OF_TIME_MOVE 1
#define VERTICAL_WALL 0
#define HORIZONTAL_WALL 1
#define MAX_NUM_WALLS 8
#define MESSAGE_LENGTH 30


typedef struct{
	Coordinates position[MAX_NUM_WALLS];
	int dir[MAX_NUM_WALLS];
	int used;
} wallType;

typedef struct {
	int mode;
	int timeRemaining;
	int player;
	int pendingWall;
	int lastMove;
	int validMove;
	Coordinates currentPos[2];
	wallType walls[2];
} MatchType;

void initGame(void);
void setMode(int modeValue);
void setPlayer(int playerValue);
void setColorMove(Coordinates pos, int color);
void setNextPos(int h, int v);
void move(void);
void saveMove(int playerId, int moveType, int wallOrientation, Coordinates destPos);
void newWall(Coordinates centerPos, int direction);
void rotateWall(void);
void confirmWall(void);
void undoWall(void);
void setNextWall(int h, int v);

#endif
