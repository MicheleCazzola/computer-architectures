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
#define MAX_NUM_WALLS 8
#define START_X_PLAYER1 3
#define START_Y_PLAYER1 6
#define START_X_PLAYER2 3 
#define START_Y_PLAYER2 0


typedef struct {
	int x;
	int y;
} Coordinates;

typedef struct{
	Coordinates position[MAX_NUM_WALLS];
	int dir[8];
	int used;
} wallType;

typedef struct {
	int mode;
	int time_remaining;
	int player;
	int pending_wall;
	int last_move;
	int validMove;
	Coordinates currentPos[2];
	wallType walls[2];
} PlayType;

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