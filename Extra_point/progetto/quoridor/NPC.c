#include "quoridor.h"
#include "stdlib.h"
#include "../graphics/interface.h"
#include "../queue/queue.h"

#define DOWN 0
#define LEFT_DOWN 1
#define LEFT 2
#define LEFT_UP 3
#define UP 4
#define RIGHT_UP 5
#define RIGHT 6
#define RIGHT_DOWN 7

#define MAX_DIST (NUM_SQUARES * NUM_SQUARES)

// Possibili mosse
const Coordinates POSSIBLE_MOVES[8] = {
	{0, 1},								// DOWN
	{-1, 1},							// LEFT-DOWN
	{-1, 0},							// LEFT
	{-1, -1},							// LEFT-UP
	{0, -1},							// UP
	{1, -1},							// RIGHT-UP
	{1, 0},								// RIGHT
	{1, 1}								// RIGHT-DOWN
};

// Colore dei giocatori
extern const int PLAYER_COLORS[2];

// Check se mossa specifica � tra le selezinate
static int moveSelected(Coordinates *selected, char numSel, Coordinates *currentPos, char direction){
	int i;
	
	// Coordinate (x,y) dello spostamento
	Coordinates move = POSSIBLE_MOVES[direction];
	
	for(i = 0; i < numSel; i++){
		
		// Se la destinazione scelta � tra le selezionate, si termina la ricerva
		if(equalCoord(selected[i], changeCoord(*currentPos, move.x, move.y))){
			return 1;
		} 
	}
	
	return 0;
}

// Misura distanza tra posizione del giocatore e destinazione
// Non considera presenza giocatore avversario (versione successiva dovrebbe farlo)
// Parametri:
// -	ms: stato del gioco
// -	player: giocatore di riferimento
// Risultato:
// -	primario: 1 se arriva a destinazione, 0 altrimenti
// -	secondario: distanza dalla destinazione pi� vicina
static int measureDistance(Coordinates startPlayerPos, char player, int *dist){
	int i, j;
	
	// Coda FIFO implementata con buffer circolare
	// per avere QUEUE_DIM < NUM_SQUARES^2 = 49
	Coordinates queue[QUEUE_DIM];
	
	// Matrice di distanza, usata anche con funzione di visita
	char distance[NUM_SQUARES][NUM_SQUARES];
	
	// Variabili di tipo (x,y) per agire sulle celle
	Coordinates currElem, adjElem;
	
	// Inizializzazione matrice di distanza
	for(i = 0; i < NUM_SQUARES; i++){
		for(j = 0; j < NUM_SQUARES; j++){
			distance[i][j] = MAX_DIST;
		}
	}
	
	// Inizializzazione coda FIFO e
	// inserimento posizione corrente del giocatore
	initQueue(queue);
	currElem = startPlayerPos;
	enqueue(queue, currElem);
	
	// Impostazione distanza sorgente
	distance[currElem.x][currElem.y] = 0;
	
	// La ricerca procede, nel caso negativo,
	// fino allo svuotamento della coda
	while(!isEmpty(queue)){
		currElem = dequeue(queue);
		
		// Check vittoria: salvataggio distanza (minima)
		if(victory(currElem, player)){
			*dist = distance[currElem.x][currElem.y];
			return 1;
		}
		
		// Check raggiungibilit� destinazione, senza considerare
		// la posizione corrente dell'avversario
		// Iterazione sulle celle adiacenti
		for(i = 0; i < 8; i += 2){
			
			// Set adiacenza corrente
			adjElem = changeCoord(currElem, POSSIBLE_MOVES[i].x, POSSIBLE_MOVES[i].y);
			
			if(validPos(adjElem, currElem)){
				// Se la cella non � in coda, si inserisce e si aggiorna la distanza
				if(distance[adjElem.x][adjElem.y] == MAX_DIST){
					enqueue(queue, adjElem);
					distance[adjElem.x][adjElem.y] = distance[currElem.x][currElem.y] + 1;
				}
			}
		}
	}
	
	return 0;
}

// Scelta mossa da effettuare con criterio di ottimizzazione locale
// Metrica: max(dist(otherPlayer, destination) - dist(currentPlayer, destination))
// Risultato: 0 se conviene (o si � obbligati a) muovere, 1 altrimenti
static int chooseMove(MatchType *ms, Coordinates *nextPosM, Coordinates *nextPosW, char *nextWDir){
	
	int i, j, dir, maxDiffMove, maxDiffWall, localDistCurrent, localDistOther;
	char currentPlayer, otherPlayer, numSel, nextDirWall, ragg;
	Coordinates currentPos, otherPos, wallPos, nextPosMove, nextPosWall;
	Coordinates selected[5];
	
	// Giocatori
	currentPlayer = ms->player;
	otherPlayer = getOtherPlayer(ms->player);
	
	// Posizione corrente dei giocatori
	currentPos = ms->currentPos[currentPlayer];
	otherPos = ms->currentPos[otherPlayer];
	
	// Selezione celle valide in cui muoversi
	numSel = 0;
	selectAdj(&currentPos, currentPlayer, selected, &numSel);
	
	// Calcolo distanza avversario-destinazione, per compatibilit�
	// con confronto tra muri
	measureDistance(otherPos, otherPlayer, &localDistOther);
	
	// Inizializzazione metrica a valore minimo
	maxDiffMove = -MAX_DIST;
	
	// Iterazione su mosse possibili
	// Non si considera l'avversario, dato che la measureDistance v1.0
	// non lo considera nel calcolo del costo -> Distanza costante,
	// si minimizza quella del giocatore corrente
	for(i = 0; i < numSel; i++){
		
		// Minima locale
		measureDistance(selected[i], currentPlayer, &localDistCurrent);
		
		// Check metrica migliorata
		if(localDistOther - localDistCurrent > maxDiffMove){
			maxDiffMove = localDistOther - localDistCurrent;
			nextPosMove = selected[i];
		}
	}
	
	// Salvataggio risultato
	*nextPosM = nextPosMove;
	
	// Inizializzazione metrica a valore nullo (e inferiore al minimo)
	maxDiffWall = -MAX_DIST;
	
	// Check per posizionamento muri
	if(ms->walls[currentPlayer].used < MAX_NUM_WALLS){
		
		// Set flag muro in corso di verifica
		ms->pendingWall = 1;
		
		// Iterazione su muri possibili
		for(i = 0; i < NUM_SQUARES-1; i++){
			for(j = 0; j < NUM_SQUARES-1; j++){
				for(dir = 0; dir <= 1; dir++){
					
					// Inizializzazione flag raggiungibilit�
					ragg = 1;
					
					// Centro nuovo muro
					wallPos = newCoord(i, j);
					
					// MURO VERTICALE: inserimento
					setWall(wallPos, dir);
					
					// Check muro valido (non sovrapposizione con altri)
					if(checkNotOverlapping(wallPos, dir)){
						
						// Misura distanze, con check raggiungibilit�
						ragg = ragg && measureDistance(currentPos, currentPlayer, &localDistCurrent);
						ragg = ragg && measureDistance(otherPos, otherPlayer, &localDistOther);
						
						// Check metrica migliorata
						if(ragg){
							if(localDistOther - localDistCurrent > maxDiffWall){
								maxDiffWall = localDistOther - localDistCurrent;
								nextPosWall = wallPos;
								nextDirWall = dir;
							}
						}
					}
				}
			}
		}
		
		// Salvataggio risultato
		*nextPosW = nextPosWall;
		*nextWDir = nextDirWall;
	}
	
	return (maxDiffMove <= maxDiffWall);
}

// Conferma mossa senza cambio giocatore e senza cancellazione adiacenze
// Risultato: 1 se partita terminata, 0 altrimenti
char moveNPC(MatchType *ms, Coordinates *nextPos){
	
	// Cancellazione token
	drawSquareArea(ms->currentPos[ms->player].x, ms->currentPos[ms->player].y, BGCOLOR);
	
	// Disegno pedina in posizione finale
	drawToken(nextPos->x, nextPos->y, PLAYER_COLORS[ms->player]);
	
	// Aggiornamento posizione finale
	ms->currentPos[ms->player] = *nextPos;
	
	// Salvataggio mossa
	saveMove(ms->player, PLAYER_MOVE, PLAYER_MOVE, &(ms->currentPos[ms->player]));
		
	return victory(ms->currentPos[ms->player], ms->player);
}

// Conferma muro senza verifica e senza cambio giocatore
void confirmWallNPC(MatchType *ms){
	
	int i;
	
	i = ms->walls[ms->player].used;
	
	// Muro confermato
	ms->pendingWall = 0;
			
	// Incremento numero muri usati
	ms->walls[ms->player].used++;
		
	// Salvataggio mossa
	saveMove(ms->player, WALL_PLACEMENT, ms->walls[ms->player].dir[i],
			&(ms->walls[ms->player].position[i]));
	
	// Aggiornamento numero muri disponibili per i giocatori
	writeWallsStats(getAvailableWalls(PLAYER1), getAvailableWalls(PLAYER2));
}


// VERSIONE 1: solo mosse, no muri
void NPC_playTurn(MatchType *status, ModeType *mode, Coordinates *nextPos){
	
	char choiceResult, nextPosDir, victory;
	Coordinates nextPosMove, nextPosWall;
	
	// Azzeramento flag vittoria
	victory = 0;
	
	// Scelta mossa
	choiceResult = chooseMove(status, &nextPosMove, &nextPosWall, &nextPosDir);
	
	// Mossa pedina
	if(choiceResult == 0){
		*nextPos = nextPosMove;
		victory = moveNPC(status, nextPos);
	}
	// Posizionamento muro
	else{
		newWall(nextPosWall, nextPosDir);
		confirmWallNPC(status);
	}
	
	// Aggiornamento stato mossa
	status->finishedNPCMove = 1 + victory;
}
