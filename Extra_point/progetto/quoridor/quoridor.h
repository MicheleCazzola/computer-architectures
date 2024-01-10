#ifndef _QUORIDOR_H_
#define _QUORIDOR_H_

#include "../coordinates/coordinates.h"

// Valori di modalità e giocatore
#define WAITING 0
#define PLAYING 1
#define PLAYER1 0
#define PLAYER2 1

// Numero massimo muri
#define MAX_NUM_WALLS 8

// Lunghezza massima di un messaggio, stampabile su singola riga
#define MESSAGE_LENGTH (MAX_X / 8 + 1)

// Valori per caratteristiche mossa
#define PLAYER_MOVE 0
#define WALL_PLACEMENT 1
#define OUT_OF_TIME_MOVE 1
#define VERTICAL_WALL 0
#define HORIZONTAL_WALL 1

// Muro
typedef struct{
	Coordinates position[MAX_NUM_WALLS];	// posizione del centro dei muri
	char dir[MAX_NUM_WALLS];								// direzione dei muri
	char used;															// numero di muri utilizzati, in [0, MAX_NUM_WALLS]
} WallType;

// Variabile di stato
typedef struct {
	char mode;										// modalità di gioco {0: attesa, 1: gioco}
	char timeRemaining;					// tempo rimanente, in [0, 20] s
	char player;									// indice del giocatore, pari a playerId-1
	char pendingWall;						// flag per indicare presenza muro in attesa di conferma
	int lastMove;								// intero a 32 bit per salvare l'ultima mossa effettuata
	char validMove;							// flag per indicare se l'ultimo movimento effettuato è verso una posizione valida
	char numHighlited;						// numero celle evidenziate
	Coordinates highlited[5];		// posizione celle evidenziate
	Coordinates currentPos[2];	// posizione corrente dei due giocatori
	WallType walls[2];					// muri correnti dei due giocatori
} MatchType;

void initGame(void);
void setMode(char modeValue);
void setPlayer(char playerValue);
int getOtherPlayer(char player);
void highliteAdj(Coordinates pos);
void eraseHighlightedAdj(void);
void setNextPos(int h, int v);
void move(void);
void newWall(Coordinates centerPos, char direction);
void rotateWall(void);
void confirmWall(void);
void undoWall(void);
void setNextWall(int h, int v);
void saveMove(int playerId, int moveType, int wallOrientation, Coordinates *destPos);

#endif
