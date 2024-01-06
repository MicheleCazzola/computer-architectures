#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "../GLCD/GLCD.h"

// Colore oggeti
#define BGCOLOR Blue2
#define VALID_MOVE_COLOR Green
#define GRID_COLOR Black
#define PLAYER1_COLOR White
#define PLAYER2_COLOR Red
#define TIME_COLOR White
#define TEXT_COLOR White
#define MESSAGE_COLOR White

// Dimensione oggetti
#define NUM_SQUARES 7
#define NUM_BOXES 3
#define SQUARE_SIDE 30
#define BOX_WIDTH 76
#define BOX_HEIGHT 50
#define TOKEN_SIDE 11
#define WALL_LENGTH (2 * SQUARE_SIDE + SPACE_WIDTH)

// Spaziature oggetti
#define MARGIN_WIDTH 3
#define SPACE_WIDTH 4
#define BOX_SPACE_MARGIN_WIDTH 3

// Coordinate orizzontali testi
#define XPOS_TEXT1 (MARGIN_WIDTH + BOX_WIDTH/2 - 28)
#define XPOS_WALLS1 (MARGIN_WIDTH + BOX_WIDTH/2 - 4)
#define XPOS_TEXT2 (XPOS_TEXT1 + 2 * BOX_SPACE_MARGIN_WIDTH + 2 * BOX_WIDTH)
#define XPOS_WALLS2 (XPOS_WALLS1 + 2 * BOX_SPACE_MARGIN_WIDTH + 2 * BOX_WIDTH)
#define XPOS_TIME (2 * BOX_SPACE_MARGIN_WIDTH + BOX_WIDTH + BOX_WIDTH/2 - 16)
#define XPOS_MESSAGE (MARGIN_WIDTH)

// Coordinate verticali testi
#define PADDING_TOP (10 + MARGIN_WIDTH + 6*SPACE_WIDTH + NUM_SQUARES*SQUARE_SIDE)
#define YPOS_TIME (PADDING_TOP + 20)
#define YPOS_TEXTS (PADDING_TOP + 10)
#define YPOS_WALLS (YPOS_TEXTS + 15)
#define YPOS_MESSAGE 305

// Intestazione box con statistiche
#define PLAYER1_MSG "P1 Wall"
#define PLAYER2_MSG "P2 Wall"

// Valore direzione muri
#define VERTICAL_WALL 0
#define HORIZONTAL_WALL 1

void drawChessPlatform(void);
void drawToken(int x, int y, int color);
void writeWallsStats(int wall_p1, int wall_p2);
void writeTimeRemaining(int time, int color);
void drawSquareArea(int x, int y, int color);
void drawWall(int x, int y, int direction, int color);
void clearMessage(void);
void writeMessage(char message_content[]);

#endif

