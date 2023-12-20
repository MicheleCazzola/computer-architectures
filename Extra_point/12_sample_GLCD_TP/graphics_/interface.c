/*
	Disegno scacchiera
	7 quadrati per ogni dimensione + 6 spazi + 2 margini -> 7*q_side + 6*space_dim + 2*margin = 240px
	Q_size = 30px
	Space_dim = 4px
	Margin_dim = 3px
	
*/
#include "interface.h"
#include "../GLCD/GLCD.h"

void drawChessPlatform(){
	int i, j;
	for(i = 0; i < NUM_SQUARES; i++){
		for(j = 0; j < NUM_SQUARES; j++){
			drawSquare(i, j);
		}
	}
}

void drawSquare(int row, int col){
	
}