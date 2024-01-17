#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "../GLCD/GLCD.h"

// Messaggio corrente da stampare
extern char message[];

// Disegno scritta generica
static void drawText(const char *text, const int topY){
	int n;
	
	n = 8 * strlen(text);
	GUI_Text((MAX_X - n)/2, topY, (unsigned char *) text, TEXTCOLOR_MENU, BGCOLOR_MENU);
}

// Disegno riquadro
void drawOptionBorder(int startX, int finalX, int topY, int color){
	int bottomY;
	
	bottomY = topY + 16 + 2 * VERT_PADDING_MENU;
	LCD_DrawLine(startX, topY, finalX, topY, color);
	LCD_DrawLine(startX, bottomY , finalX, bottomY, color);
	LCD_DrawLine(startX, topY, startX, bottomY, color);
	LCD_DrawLine(finalX, topY, finalX, bottomY, color);
}

// Disegno opzione
static void drawOption(const char *text, const int startX, const int finalX, const int topY, int boardColor){
	int n;
	
	n = 8 * strlen(text);
	drawOptionBorder(startX, finalX, topY, boardColor);
	GUI_Text((MAX_X - n)/2, topY + VERT_PADDING_MENU, (unsigned char *) text, TEXTCOLOR_MENU, BGCOLOR_MENU); 
}

// Disegno menu
void drawMenu(const char *questionFirstLine, const char *questionSecondLine, const char *option1, const char *option2){
	LCD_Clear(White);
	drawText(questionFirstLine, YPOS_Q1);
	drawText(questionSecondLine, YPOS_Q2);
	drawOption(option1, XPOS_START_OPTION, XPOS_END_OPTION, YPOS_OPT1, SELECTED_BOARD_COLOR);
	drawOption(option2, XPOS_START_OPTION, XPOS_END_OPTION, YPOS_OPT2, BOARD_COLOR);
}

// Evidenziazione bordo opzione menu
void highliteChoice(int choice){
	const int y1 = (choice == 0) ? YPOS_OPT1 : YPOS_OPT2;
	const int y2 = (choice == 0) ? YPOS_OPT2 : YPOS_OPT1;
	drawOptionBorder(XPOS_START_OPTION, XPOS_END_OPTION, y1, SELECTED_BOARD_COLOR);
	drawOptionBorder(XPOS_START_OPTION, XPOS_END_OPTION, y2, BOARD_COLOR);
}

// Disegno schermo a stato iniziale
void drawChessPlatform(){
	int i, j, x, y;
	
	// Stampa cacchiera
	for(i = 0, y = MARGIN_WIDTH; i < NUM_SQUARES; i++, y += SQUARE_SIDE + SPACE_WIDTH){
		for(j = 0, x = MARGIN_WIDTH; j < NUM_SQUARES; j++, x += SQUARE_SIDE + SPACE_WIDTH){
			LCD_DrawLine(x, y, x + SQUARE_SIDE, y, GRID_COLOR);
			LCD_DrawLine(x, y + SQUARE_SIDE, x + SQUARE_SIDE, y + SQUARE_SIDE, GRID_COLOR);
			LCD_DrawLine(x, y, x, y + SQUARE_SIDE, GRID_COLOR);
			LCD_DrawLine(x + SQUARE_SIDE, y, x + SQUARE_SIDE, y + SQUARE_SIDE, GRID_COLOR);
		}
	}
	
	// Stampa contorni dei box con statistiche e tempo rimanente
	y = PADDING_TOP + BOX_SPACE_MARGIN_WIDTH;
	for(i = 0, x = BOX_SPACE_MARGIN_WIDTH; i < NUM_BOXES; i++, x += BOX_WIDTH + BOX_SPACE_MARGIN_WIDTH){
		LCD_DrawLine(x, y, x + BOX_WIDTH, y, GRID_COLOR);
		LCD_DrawLine(x, y + BOX_HEIGHT, x + BOX_WIDTH, y + BOX_HEIGHT, GRID_COLOR);
		LCD_DrawLine(x, y, x, y + BOX_HEIGHT, GRID_COLOR);
		LCD_DrawLine(x + BOX_WIDTH, y, x + BOX_WIDTH, y + BOX_HEIGHT, GRID_COLOR);
	}
}

// Disegno bordo pedina
void drawTokenBorder(int x, int y, int color){
	int x0, x1, y0, y1;
	
	// Calcolo coordinate utili
	x0 = MARGIN_WIDTH + (SQUARE_SIDE - TOKEN_SIDE)/2 + x * (SPACE_WIDTH + SQUARE_SIDE) + 1;
	x1 = x0 + TOKEN_SIDE - 2;
	y0 = MARGIN_WIDTH + (SQUARE_SIDE - TOKEN_SIDE)/2 + y * (SPACE_WIDTH + SQUARE_SIDE) + 1;
	y1 = y0 + TOKEN_SIDE - 2;
	
	LCD_DrawLine(x0, y0, x1, y0, color);
	LCD_DrawLine(x0, y0, x0, y1, color);
	LCD_DrawLine(x1, y0, x1, y1, color);
	LCD_DrawLine(x0, y1, x1, y1, color);
}

// Disegno pedina (quadrato centrato nella casella)
void drawToken(int x, int y, int color){
	int xp, yp, i, j;
	
	// Disegno bordo
	drawTokenBorder(x, y, TOKEN_BORDER_COLOR);
	
	// Calcolo centro pedine
	xp = MARGIN_WIDTH + x * (SPACE_WIDTH + SQUARE_SIDE) + (SQUARE_SIDE)/2;
	yp = MARGIN_WIDTH + y * (SPACE_WIDTH + SQUARE_SIDE) + (SQUARE_SIDE)/2;
	
	// Scrittura per linee, centrata in (xp, yp)
	for(i = -TOKEN_SIDE/2 + 1; i < TOKEN_SIDE/2 - 1; i++){
		for(j = -TOKEN_SIDE/2 + 1; j < TOKEN_SIDE/2 - 1; j++){
			LCD_SetPoint(xp+i, yp+j, color);
		}
	}
}

// Scrittura statistiche sui muri dei giocatori
void writeWallsStats(int wall_p1, int wall_p2){
	unsigned char s1, s2;
	
	// Numero di muri rimanenti per giocatore
	s1 = (unsigned char) (wall_p1 + '0');
	s2 = (unsigned char) (wall_p2 + '0');
	
	// Stampa intestazione e numero muri rimanenti per giocatore
	GUI_Text(XPOS_TEXT1, YPOS_TEXTS, (unsigned char *) PLAYER1_MSG, TEXT_COLOR, BGCOLOR);
	GUI_Text(XPOS_WALLS1, YPOS_WALLS, (unsigned char *) &(s1), TEXT_COLOR, BGCOLOR);
	GUI_Text(XPOS_TEXT2, YPOS_TEXTS, (unsigned char *) PLAYER2_MSG, TEXT_COLOR, BGCOLOR);
	GUI_Text(XPOS_WALLS2, YPOS_WALLS, (unsigned char *) &(s2), TEXT_COLOR, BGCOLOR);
}

// Scrittura tempo rimanente
void writeTimeRemaining(int time, int color){
	char s[5] = "\0\0\0\0\0";
	
	// Scrittura tempo rimanente su stringa
	s[0] = time / 10 + '0';
	s[1] = time % 10 + '0';
	strcat(s, " s");
	
	// Stampa tempo
	GUI_Text(XPOS_TIME, YPOS_TIME, (unsigned char *) s, color, BGCOLOR);
}

// Disegno area interna al quadrato
void drawSquareArea(int x, int y, int color){
	int x0, y0, x1, cnt;
	
	// Calcolo coordinate utili
	x0 = MARGIN_WIDTH + x * (SPACE_WIDTH + SQUARE_SIDE) + 1;
	x1 = x0 + SQUARE_SIDE - 2;
	y0 = MARGIN_WIDTH + y * (SPACE_WIDTH + SQUARE_SIDE) + 1;
	
	// Disegno per linee orizzontali
	for(cnt = 0; cnt < SQUARE_SIDE - 1; cnt++, y0++){
		LCD_DrawLine(x0, y0, x1, y0, color);
	}
}

// Disegno muro
void drawWall(int x, int y, char direction, int color){
	int i, x0, x1, y0, y1, xc, yc;
	
	// Calcolo centro dei muri
	xc = MARGIN_WIDTH + SQUARE_SIDE + x * (SQUARE_SIDE + SPACE_WIDTH) + 2;
	yc = MARGIN_WIDTH + SQUARE_SIDE + y * (SQUARE_SIDE + SPACE_WIDTH) + 2;
	
	// Disegno dipendente dalla direzione
	// Orizzontale
	if(direction == HORIZONTAL_WALL){
		x0 = xc - WALL_LENGTH/2;
		x1 = xc + WALL_LENGTH/2;
		y0 = yc - (SPACE_WIDTH-1)/2;
		for(i = 0; i < SPACE_WIDTH-1; i++){
			LCD_DrawLine(x0, y0+i, x1, y0+i, color);
		}
	}
	// Verticale
	else{
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
	
	// Check esistenza
	if(strlen(message) == 0){
		return;
	}
	
	// Stessi criteri della scrittura
	if(strlen(message) < MAX_X / 8){
		GUI_Text(XPOS_MESSAGE, YPOS_MESSAGE, (unsigned char *) message, BGCOLOR, BGCOLOR);
	} 
	else{
		GUI_Text(0, YPOS_MESSAGE, (unsigned char *) message, BGCOLOR, BGCOLOR);
	}
	
	message[0] = '\0';
}

// Scrittura messaggio: si suppone che sia stampabile su una riga sola,
// ovvero che abbia al massimo 30 caratteri (terminatore escluso)
void writeMessage(char message_content[]){
	strcpy(message, message_content);
	
	// Se il messaggio ha meno di 30 caratteri ->
	// Si stampa il messaggio con lo stesso margine sinistro degli oggetti
	if(strlen(message) < MAX_X / 8){
		GUI_Text(XPOS_MESSAGE, YPOS_MESSAGE, (unsigned char *) message, MESSAGE_COLOR, BGCOLOR);
	} 
	// Altrimenti -> Si stampa senza margine sinistro
	else{
		GUI_Text(0, YPOS_MESSAGE, (unsigned char *) message, MESSAGE_COLOR, BGCOLOR);
	}
}

// Disegno riga per multiplayer
void drawPlayerColor(int color){
	int i;
	
	for(i = 0; i < NUM_PLAYER_LINES; i++){
		LCD_DrawLine(MARGIN_WIDTH, YPOS_PLAYER_COLOR + i, MAX_X - MARGIN_WIDTH, YPOS_PLAYER_COLOR + i, color);
	}
}