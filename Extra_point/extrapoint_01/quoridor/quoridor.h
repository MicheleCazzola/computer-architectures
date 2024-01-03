#ifndef _QUORIDOR_H_
#define _QUORIDOR_H_

#include "../coordinates/coordinates.h"

// Valori di modalità e giocatore
#define WAITING 0
#define PLAYING 1
#define PLAYER1 1
#define PLAYER2 2

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
	int dir[MAX_NUM_WALLS];								// direzione dei muri
	int used;															// numero di muri utilizzati, in [0, MAX_NUM_WALLS]
} wallType;

// Variabile di stato
typedef struct {
	int mode;										// modalità di gioco {0: attesa, 1: gioco}
	int timeRemaining;					// tempo rimanente, in [0, 20] s
	int player;									// indice del giocatore, pari a playerId-1
	int pendingWall;						// flag per indicare presenza muro in attesa di conferma
	int lastMove;								// intero a 32 bit per salvare l'ultima mossa effettuata
	int validMove;							// flag per indicare se l'ultimo movimento effettuato è verso una posizione valida
	Coordinates currentPos[2];	// posizione corrente dei due giocatori
	wallType walls[2];					// muri correnti dei due giocatori
} MatchType;

void initGame(void);
void setMode(int modeValue);
void setPlayer(int playerValue);
void setColorMove(Coordinates pos, int color);
int equalCoordinates(Coordinates a, Coordinates b);
Coordinates getNextPos(void);
void setNextPos(int h, int v);
void move(void);
void saveMove(int playerId, int moveType, int wallOrientation, Coordinates *destPos);
void newWall(Coordinates centerPos, int direction);
void rotateWall(void);
void confirmWall(void);
void undoWall(void);
void setNextWall(int h, int v);

#endif
