#ifndef _QUORIDOR_H_
#define _QUORIDOR_H_

#define WAITING 0
#define PLAYING 1
#define PLAYER1 1
#define PLAYER2 2
#define MAX_MOVES_NUMBER 100
#define PLAYER_MOVE 0
#define WALL_PLACEMENT 1
#define OUT_OF_TIME_MOVE 1
#define VERTICAL_WALL 0
#define HORIZONTAL_WALL 1
#define WALL_DEFAULT_ROW 2
#define WALL_DEFAULT_COL 3

typedef struct{
	int rowC[8];
	int colC[8];
	int dir[8];
	int used;
} wallType;

void initGame(void);
void setMode(int modeValue);
void setPlayer(int playerValue);
void setColorMove(int row, int col, int color);
void setNextPos(int h, int v);
void move(void);
void saveMove(int playerId, int moveType, int wallOrientation, int y, int x);
void newWall(int Xcenter, int Ycenter, int direction);
void undoWall();
void rotateWall();
void confirmWall();
void setNextWall(int h, int v);

#endif