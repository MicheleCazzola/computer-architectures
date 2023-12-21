/*
	Disegno scacchiera
	7 quadrati per ogni dimensione + 6 spazi + 2 margini -> 7*q_side + 6*space_dim + 2*margin = 240px
	Q_size = 30px
	Space_dim = 4px
	Margin_dim = 3px
	
*/

#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "../GLCD/GLCD.h"

void drawChessPlatform(){
	int i, j;
	for(i = 0; i < NUM_SQUARES; i++){
		for(j = 0; j < NUM_SQUARES; j++){
			drawSquare(i, j);
		}
	}
	
	for(i = 0; i < NUM_BOXES; i++){
		drawBox(i);
	}
}

void drawSquare(int row, int col){
	int x0, y0, x1, y1;
	
	// Upper horiz
	x0 = MARGIN_WIDTH + col * (SPACE_WIDTH + SQUARE_SIDE);
	x1 = x0 + SQUARE_SIDE;
	y0 = MARGIN_WIDTH + row * (SPACE_WIDTH + SQUARE_SIDE);
	y1 = y0;
	LCD_DrawLine(x0, y0, x1, y1, White);
	
	// Lower horiz
	x0 = MARGIN_WIDTH + col * (SPACE_WIDTH + SQUARE_SIDE);
	x1 = x0 + SQUARE_SIDE;
	y0 = MARGIN_WIDTH + row * (SPACE_WIDTH + SQUARE_SIDE) + SQUARE_SIDE;
	y1 = y0;
	LCD_DrawLine(x0, y0, x1, y1, White);
	
	// Left vert
	x0 = MARGIN_WIDTH + col * (SPACE_WIDTH + SQUARE_SIDE);
	x1 = x0;
	y0 = MARGIN_WIDTH + row * (SPACE_WIDTH + SQUARE_SIDE);
	y1 = y0 + SQUARE_SIDE;
	LCD_DrawLine(x0, y0, x1, y1, White);
	
	// Right vert
	x0 = MARGIN_WIDTH + col * (SPACE_WIDTH + SQUARE_SIDE) + SQUARE_SIDE;
	x1 = x0;
	y0 = MARGIN_WIDTH + row * (SPACE_WIDTH + SQUARE_SIDE);
	y1 = y0 + SQUARE_SIDE;
	LCD_DrawLine(x0, y0, x1, y1, White);
}

void drawBox(int index){
	int x0, y0, x1, y1;
	
	// Upper horiz
	x0 = MARGIN_WIDTH + index * (BOX_WIDTH + SPACE_WIDTH);
	x1 = x0 + BOX_WIDTH;
	y0 = PADDING + MARGIN_WIDTH;
	y1 = y0;
	LCD_DrawLine(x0, y0, x1, y1, White);
	
	// Lower horiz
	x0 = MARGIN_WIDTH + index * (BOX_WIDTH + SPACE_WIDTH);
	x1 = x0 + BOX_WIDTH;
	y0 = PADDING + MARGIN_WIDTH + BOX_HEIGHT;
	y1 = y0;
	LCD_DrawLine(x0, y0, x1, y1, White);
	
	// Left vert
	x0 = MARGIN_WIDTH + index * (BOX_WIDTH + SPACE_WIDTH);
	x1 = x0;
	y0 = PADDING + MARGIN_WIDTH;
	y1 = y0 + BOX_HEIGHT;
	LCD_DrawLine(x0, y0, x1, y1, White);
	
	// Right vert
	x0 = MARGIN_WIDTH + index * (BOX_WIDTH + SPACE_WIDTH) + BOX_WIDTH;
	x1 = x0;
	y0 = PADDING + MARGIN_WIDTH;
	y1 = y0 + BOX_HEIGHT;
	LCD_DrawLine(x0, y0, x1, y1, White);
}

void drawToken(int row, int col, int color){
	int xp, yp, i, j;
	xp = MARGIN_WIDTH + col * (SPACE_WIDTH + SQUARE_SIDE) + (SQUARE_SIDE)/2;
	yp = MARGIN_WIDTH + row * (SPACE_WIDTH + SQUARE_SIDE) + (SQUARE_SIDE)/2;
	for(i = -TOKEN_SIDE/2; i < TOKEN_SIDE/2; i++){
		for(j = -TOKEN_SIDE/2; j < TOKEN_SIDE/2; j++){
			LCD_SetPoint(xp+i, yp+j, color);
		}
	}
}

void writeWalls(int wall_p1, int wall_p2){
	unsigned char s1 = (unsigned char) (wall_p1 + '0');
	unsigned char s2 = (unsigned char) (wall_p2 + '0');
	GUI_Text(XPOS_TEXT1, ROW1, PLAYER1_MSG, White, Blue);
	GUI_Text(XPOS_TEXT1, ROW2, (unsigned char *) &(s1), White, Blue);
	GUI_Text(XPOS_TEXT2, ROW1, PLAYER2_MSG, White, Blue);
	GUI_Text(XPOS_TEXT2, ROW2, (unsigned char *) &(s2), White, Blue);
}

void writeTimeRemaining(int time){
	unsigned char s[5];
	s[0] = time / 10 + '0';
	s[1] = time % 10 + '0';
	s[2] = ' ';
	s[3] = 's';
	s[4] = '\0';
	GUI_Text(XPOS_TIME, ROW_TIME, s, White, Blue);
}

void drawSquareArea(int r, int c, int color){
	int x0, y0, x1, y1, cnt;
	
	// Upper horiz
	x0 = MARGIN_WIDTH + c * (SPACE_WIDTH + SQUARE_SIDE) + 1;
	x1 = x0 + SQUARE_SIDE - 2;
	y0 = MARGIN_WIDTH + r * (SPACE_WIDTH + SQUARE_SIDE) + 1;
	y1 = y0;
	
	for(cnt = 0; cnt < SQUARE_SIDE - 1; cnt++, y0++, y1++){
		LCD_DrawLine(x0, y0, x1, y1, color);
	}
	
}