#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "../GLCD/GLCD.h"


#define NUM_SQUARES 7
#define NUM_BOXES 3
#define SQUARE_SIDE 30
#define MARGIN_WIDTH 3
#define SPACE_WIDTH 4
#define BOX_WIDTH 76
#define BOX_HEIGHT 50
#define BOX_SPACE_MARGIN_WIDTH 3
#define PADDING (10 + MARGIN_WIDTH + 6*SPACE_WIDTH + NUM_SQUARES*SQUARE_SIDE)
#define PLAYER1 White
#define PLAYER2 Red
#define TOKEN_SIDE 11
#define XPOS_TEXT1 (MARGIN_WIDTH + 5)
#define XPOS_TEXT2 (XPOS_TEXT1 + 2 * BOX_SPACE_MARGIN_WIDTH + 2 * BOX_WIDTH)
#define ROW1 (PADDING + 10)
#define ROW2 (ROW1 + 15)
#define PLAYER1_MSG "P1 Wall"
#define PLAYER2_MSG "P2 Wall"

void drawChessPlatform(void);
void drawSquare(int row, int col);
void drawBox(int i);
void drawToken(int i, int j, int color);
void writeWalls(int wall_p1, int wall_p2);

#endif
