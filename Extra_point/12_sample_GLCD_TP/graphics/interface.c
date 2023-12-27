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

extern char message[];

// Disegno schermo a stato iniziale
void drawChessPlatform(){
	int i, j, x, y;
	
	// Scacchiera
	for(i = 0, y = MARGIN_WIDTH; i < NUM_SQUARES; i++, y += SQUARE_SIDE + SPACE_WIDTH){
		for(j = 0, x = MARGIN_WIDTH; j < NUM_SQUARES; j++, x += SQUARE_SIDE + SPACE_WIDTH){
			LCD_DrawLine(x, y, x + SQUARE_SIDE, y, GRID_COLOR);
			LCD_DrawLine(x, y + SQUARE_SIDE, x + SQUARE_SIDE, y + SQUARE_SIDE, GRID_COLOR);
			LCD_DrawLine(x, y, x, y + SQUARE_SIDE, GRID_COLOR);
			LCD_DrawLine(x + SQUARE_SIDE, y, x + SQUARE_SIDE, y + SQUARE_SIDE, GRID_COLOR);
		}
	}
	
	// Box con statistiche
	y = PADDING + BOX_SPACE_MARGIN_WIDTH;
	for(i = 0, x = BOX_SPACE_MARGIN_WIDTH; i < NUM_BOXES; i++, x += BOX_WIDTH + BOX_SPACE_MARGIN_WIDTH){
		LCD_DrawLine(x, y, x + BOX_WIDTH, y, GRID_COLOR);
		LCD_DrawLine(x, y + BOX_HEIGHT, x + BOX_WIDTH, y + BOX_HEIGHT, GRID_COLOR);
		LCD_DrawLine(x, y, x, y + BOX_HEIGHT, GRID_COLOR);
		LCD_DrawLine(x + BOX_WIDTH, y, x + BOX_WIDTH, y + BOX_HEIGHT, GRID_COLOR);
	}
}

// Disegno pedina (quadrato centrato nella casella)
void drawToken(Coordinates pos, int color){
	int xp, yp, i, j;
	xp = MARGIN_WIDTH + pos.x * (SPACE_WIDTH + SQUARE_SIDE) + (SQUARE_SIDE)/2;
	yp = MARGIN_WIDTH + pos.y * (SPACE_WIDTH + SQUARE_SIDE) + (SQUARE_SIDE)/2;
	for(i = -TOKEN_SIDE/2; i < TOKEN_SIDE/2; i++){
		for(j = -TOKEN_SIDE/2; j < TOKEN_SIDE/2; j++){
			LCD_SetPoint(xp+i, yp+j, color);
		}
	}
}

// Scrittura statistiche sui muri dei giocatori
void writeWallsStats(int wall_p1, int wall_p2){
	unsigned char s1 = (unsigned char) (wall_p1 + '0');
	unsigned char s2 = (unsigned char) (wall_p2 + '0');
	GUI_Text(XPOS_TEXT1, ROW1, (unsigned char *) PLAYER1_MSG, TEXT_COLOR, BGCOLOR);
	GUI_Text(XPOS_WALLS1, ROW2, (unsigned char *) &(s1), TEXT_COLOR, BGCOLOR);
	GUI_Text(XPOS_TEXT2, ROW1, (unsigned char *) PLAYER2_MSG, TEXT_COLOR, BGCOLOR);
	GUI_Text(XPOS_WALLS2, ROW2, (unsigned char *) &(s2), TEXT_COLOR, BGCOLOR);
}

// Scrittura tempo rimanente
void writeTimeRemaining(int time, int color){
	unsigned char s[5];
	s[0] = time / 10 + '0';
	s[1] = time % 10 + '0';
	s[2] = ' ';
	s[3] = 's';
	s[4] = '\0';
	GUI_Text(XPOS_TIME, ROW_TIME, s, color, BGCOLOR);
}

// Disegno area interna al quadrato
void drawSquareArea(Coordinates pos, int color){
	int x0, y0, x1, cnt;
	
	x0 = MARGIN_WIDTH + pos.x * (SPACE_WIDTH + SQUARE_SIDE) + 1;
	x1 = x0 + SQUARE_SIDE - 2;
	y0 = MARGIN_WIDTH + pos.y * (SPACE_WIDTH + SQUARE_SIDE) + 1;
	for(cnt = 0; cnt < SQUARE_SIDE - 1; cnt++, y0++){
		LCD_DrawLine(x0, y0, x1, y0, color);
	}
}

// Disegno muro
void drawWall(Coordinates pos, int direction, int color){
	int i, x0, x1, y0, y1, xc, yc;
	xc = MARGIN_WIDTH + SQUARE_SIDE + pos.x * (SQUARE_SIDE + SPACE_WIDTH) + 2;
	yc = MARGIN_WIDTH + SQUARE_SIDE + pos.y * (SQUARE_SIDE + SPACE_WIDTH) + 2;
	if(direction == HORIZONTAL_WALL){
		x0 = xc - WALL_LENGTH/2;
		x1 = xc + WALL_LENGTH/2;
		y0 = yc - (SPACE_WIDTH-1)/2;;
		for(i = 0; i < SPACE_WIDTH-1; i++){
			LCD_DrawLine(x0, y0+i, x1, y0+i, color);
		}
	}
	if(direction == VERTICAL_WALL){
		x0 = xc - (SPACE_WIDTH-1)/2;
		y0 = yc - WALL_LENGTH/2;
		y1 = yc + WALL_LENGTH/2;
		for(i = 0; i < SPACE_WIDTH-1; i++){
			LCD_DrawLine(x0+i, y0, x0+i, y1, color);
		}
	}
}

// Cancellazione messaggio, se esistente
void clearMessage(){
	
	if(strlen(message) == 0){
		return;
	}
	
	GUI_Text(X_MESSAGE, Y_MESSAGE, (unsigned char *) message, BGCOLOR, BGCOLOR);
	message[0] = '\0';
}

// Scrittura messaggio
void writeMessage(char message_content[]){
	strcpy(message, message_content);
	if(strlen(message) < MAX_X / 8){
		GUI_Text(X_MESSAGE, Y_MESSAGE, (unsigned char *) message, MESSAGE_COLOR, BGCOLOR);
	} 
	else{
		GUI_Text(0, Y_MESSAGE, (unsigned char *) message, MESSAGE_COLOR, BGCOLOR);
	}
}
