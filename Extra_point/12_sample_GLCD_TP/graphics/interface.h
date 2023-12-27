#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "../GLCD/GLCD.h"


#define BGCOLOR Blue
#define VALID_MOVE_COLOR Green
#define NUM_SQUARES 7
#define NUM_BOXES 3
#define SQUARE_SIDE 30
#define MARGIN_WIDTH 3
#define SPACE_WIDTH 4
#define BOX_WIDTH 76
#define BOX_HEIGHT 50
#define BOX_SPACE_MARGIN_WIDTH 3
#define GRID_COLOR White
#define PADDING (10 + MARGIN_WIDTH + 6*SPACE_WIDTH + NUM_SQUARES*SQUARE_SIDE)
#define PLAYER1_COLOR White
#define PLAYER2_COLOR Red
#define TOKEN_SIDE 11
#define XPOS_TEXT1 (MARGIN_WIDTH + BOX_WIDTH/2 - 28)
#define XPOS_WALLS1 (MARGIN_WIDTH + BOX_WIDTH/2 - 4)
#define XPOS_TEXT2 (XPOS_TEXT1 + 2 * BOX_SPACE_MARGIN_WIDTH + 2 * BOX_WIDTH)
#define XPOS_WALLS2 (XPOS_WALLS1 + 2 * BOX_SPACE_MARGIN_WIDTH + 2 * BOX_WIDTH)
#define XPOS_TIME (2 * BOX_SPACE_MARGIN_WIDTH + BOX_WIDTH + BOX_WIDTH/2 - 16)
#define ROW1 (PADDING + 10)
#define ROW2 (ROW1 + 15)
#define ROW_TIME (PADDING + 20)
#define PLAYER1_MSG "P1 Wall"
#define PLAYER2_MSG "P2 Wall"
#define WALL_LENGTH (2 * SQUARE_SIDE + SPACE_WIDTH)
#define VERTICAL_WALL 0
#define HORIZONTAL_WALL 1
#define X_MESSAGE (MARGIN_WIDTH)
#define Y_MESSAGE 305
#define MESSAGE_COLOR White
#define TIME_COLOR White
#define TEXT_COLOR White

typedef struct {
	int x;
	int y;
} Coordinates;

void drawChessPlatform(void);
void drawToken(Coordinates pos, int color);
void writeWallsStats(int wall_p1, int wall_p2);
void writeTimeRemaining(int time, int color);
void drawSquareArea(Coordinates pos, int color);
void drawWall(Coordinates pos, int direction, int color);
void writeMessage(char message_content[]);
void clearMessage(void);

#endif

