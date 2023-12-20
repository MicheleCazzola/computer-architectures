#ifndef _QUORIDOR_H_
#define _QUORIDOR_H_

#define WAITING 0
#define PLAYING 1
#define PLAYER1 1
#define PLAYER2 2

void initGame(void);
void setMode(int modeValue);
void setPlayer(int playerValue);
void highlightValidMoves(int row, int col);
void move(int h, int v);

#endif