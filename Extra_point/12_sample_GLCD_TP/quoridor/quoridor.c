#include <string.h>
#include "quoridor.h"
#include "../button/button.h"
#include "../timer/timer.h"
#include "../graphics/interface.h"
#include "../queue/queue.h"

// Posizioni (x,y) iniziali dei giocatori
const Coordinates START_POS_PLAYER1 = {3,6};
const Coordinates START_POS_PLAYER2 = {3,0};

// Posizione iniziale muro generico
const Coordinates WALL_DEFAULT_POS = {3,2};

// Colore pedina per ogni giocatore
const int PLAYER_COLORS[2] = {White, Red};

// Colore muro per ogni giocatore
const int WALL_COLORS[2] = {White, Red};

// Spostamenti consentiti consentite
const int moves[4][2] = {
	{-1, 0},
	{0, -1},
	{0, 1},
	{1, 0}
};

// Variabile di stato del gioco
MatchType ms;

// Posizione futura della pedina
Coordinates nextPos;

// Messaggio stampato a video
char message[MESSAGE_LENGTH] = "\0\0";

// Funzioni matematiche
static int abs(int x){
	return x > 0 ? x : -x;
}

static int sign(int x){
	if(x != 0){
		return x > 0 ? 1 : -1;
	}
	return x;
}

// Get avversario
static int getOtherPlayer(int player){
	return 3 - player;
}

// Check vittoria
static int victory(Coordinates pos, int player){
	return pos.y == 6 * (player-1);
}

// Check posizione dentro ai bordi
static int posInPlatform(Coordinates pos){
	return 0 <= pos.x && pos.x < NUM_SQUARES && 0 <= pos.y && pos.y < NUM_SQUARES;
}

// Check muro dentro ai bordi
static int centerWallInPlatform(Coordinates *pos){
	return 0 <= pos->x && pos->x < NUM_SQUARES-1 && 0 <= pos->y && pos->y < NUM_SQUARES-1;
}

// Get posizione finale in presenza di avversario in posizione adiacente
static void jumpOverOpponent(Coordinates *srcPos, Coordinates *destPos, int player, Coordinates *finalPos){
	int stepX, stepY;
	stepX = destPos->x - srcPos->x;
	stepY = destPos->y - srcPos->y;
	if(ms.currentPos[getOtherPlayer(player) - 1].x == destPos->x &&
				ms.currentPos[getOtherPlayer(player) - 1].y == destPos->y){
		stepX = (srcPos->y == destPos->y) ? sign(stepX) * (abs(stepX) + 1) : 0;
		stepY = (srcPos->x == destPos->x) ? sign(stepY) * (abs(stepY) + 1) : 0;
	}
	finalPos->x = srcPos->x + stepX;
	finalPos->y = srcPos->y + stepY;
}

// Check presenza di un muro specifico tra due celle
static int wallBetweenCells(Coordinates *srcPos, Coordinates *destPos, Coordinates *centerPos, int dir){
	if(dir == HORIZONTAL_WALL){
		if(srcPos->x == destPos->x && (destPos->x == centerPos->x || destPos->x == centerPos->x+1) && srcPos->y <= centerPos->y && centerPos->y < destPos->y) return 1;
		if(srcPos->x == destPos->x && (destPos->x == centerPos->x || destPos->x == centerPos->x+1) && destPos->y <= centerPos->y && centerPos->y < srcPos->y) return 1;
	}
	else{
		if(srcPos->y == destPos->y && (destPos->y == centerPos->y || destPos->y == centerPos->y+1) && srcPos->x <= centerPos->x && centerPos->x < destPos->x) return 1;
		if(srcPos->y == destPos->y && (destPos->y == centerPos->y || destPos->y == centerPos->y+1) && destPos->x <= centerPos->x && centerPos->x < srcPos->x) return 1;
	}
	return 0;
}

// Check presenza di almeno un muro tra due celle
static int noWallBetween(Coordinates adjPos, Coordinates currentPos){
	int p, i, wall_in_progress;
	for(p = 1; p < 3; p++){
		wall_in_progress = p == ms.player && ms.pendingWall == 1;
		for(i = 0; i < ms.walls[p-1].used + wall_in_progress; i++){
			if(wallBetweenCells(&currentPos, &adjPos, &(ms.walls[p-1].position[i]), ms.walls[p-1].dir[i])){
				return 0;
			}
		}
	}
	
	return 1;
}

// Check posizione valida per spostamento
static int validPos(Coordinates pos, Coordinates currentPos){
	int in_platform, connected;
	
	// Non out-of-range
	in_platform = posInPlatform(pos);
	// Assenza di muri tra le celle interessate
	connected = noWallBetween(pos, currentPos);
	
	return in_platform && connected;
}

// Check raggiungibilità, da parte di un giocatore, di almeno
// una possibile cella del lato opposto al proprio di partenza
// Si usa ricerca in ampiezza (BFS) di almeno un percorso esistente
static int checkReachability(int player){
	
	int i, j;
	Coordinates queue[NUM_SQUARES*NUM_SQUARES];
	int enqueued[NUM_SQUARES][NUM_SQUARES];
	Coordinates currElem, adjElem, finalPos;
	
	// Azzeramento flag di visita
	for(i = 0; i < NUM_SQUARES; i++){
		for(j = 0; j < NUM_SQUARES; j++){
			enqueued[i][j] = 0;
		}
	}
	
	// Inizializzazione coda FIFO e
	// inserimento posizione corrente del giocatore
	initQueue(queue);
	currElem = ms.currentPos[player-1];
	enqueue(queue, currElem);
	enqueued[currElem.x][currElem.y] = 1;
	
	// La ricerca procede, nel caso negativo,
	// fino allo svuotamento della coda
	while(!isEmpty(queue)){
		currElem = dequeue(queue);
		
		// Check vittoria
		if(victory(currElem, player)){
			clearQueue(queue);
			return 1;
		}
		
		// Iterazione sulle adiacenze:
		// si effettua eventuale correzione e validazione
		// della posizione destinazione e si inserisce
		// nella coda, marcando il flag, se essa non
		// era ancora stata inserita
		for(i = 0; i < 4; i++){
			adjElem.x = currElem.x + moves[i][0];
			adjElem.y = currElem.y + moves[i][1];
			
			jumpOverOpponent(&currElem, &adjElem, player, &finalPos);
			if(validPos(finalPos, currElem)){
						if(enqueued[finalPos.x][finalPos.y] == 0){
							enqueue(queue, finalPos);
							enqueued[finalPos.x][finalPos.y] = 1;
						}
				}
		}
	}
	
	return 0;
}

// Check esistenza di un muro di un giocatore specifico sovrapposto a quello corrente
// Si salva l'indice del muro a cui il corrente si sovrappone
static int exists_overlapping_wall(int playerWalls, Coordinates *centerPos, int dir, int *overlapping){
	int i, numWalls, overlap = 0;
	numWalls = ms.walls[playerWalls-1].used;
	for(i = 0; i < numWalls && !overlap; i++){
		if(dir == ms.walls[playerWalls-1].dir[i]){
			*overlapping = i;
			if(dir == HORIZONTAL_WALL){
				overlap = ms.walls[playerWalls-1].position[i].y == centerPos->y &&
									abs(ms.walls[playerWalls-1].position[i].x - centerPos->x) <= 1;
			}
			else{
				overlap = ms.walls[playerWalls-1].position[i].x == centerPos->x && 
									abs(ms.walls[playerWalls-1].position[i].y - centerPos->y) <= 1;
			}
		}
		else{
			*overlapping = i;
			overlap = ms.walls[playerWalls-1].position[i].x == centerPos->x &&
								ms.walls[playerWalls-1].position[i].y == centerPos->y;
		}
	} 
	
	return overlap;
}

// Check esistenza di un muro sovrapposto a quello corrente
// Si salvano l'indice del muro a cui il corrente si sovrappone e
// l'indice del giocatore a cui appartiene il muro
static int checkNotOverlapping(Coordinates *centerPos, int dir, int *overlapping, int *player){
	int overlap1, overlap2, overlapping1, overlapping2;
	overlap1 = exists_overlapping_wall(PLAYER1, centerPos, dir, &overlapping1);
	overlap2 = exists_overlapping_wall(PLAYER2, centerPos, dir, &overlapping2);
	
	if(overlap1 || overlap2){
		*player = overlap2;
		*overlapping = (overlap1 == 1) ? overlapping1 : overlapping2;
	}
	
	return !(overlap1 || overlap2);
}

// Check validazione muro
static int validWallPos(Coordinates *centerPos, int dir, int *overlapped, int *player){
	int r1, r2, not_overlap, valid_position;
	
	// Non out-of-range
	valid_position = centerWallInPlatform(centerPos);
	
	// Non impedisce possibilità di vittoria per entrambi i giocatori
	r1 = checkReachability(PLAYER1);
	r2 = checkReachability(PLAYER2);
	
	// Non si sovrappone a muri esistenti
	not_overlap = checkNotOverlapping(centerPos, dir, overlapped, player);
	
	return valid_position && r1 && r2 && not_overlap;
}

// Inserimento del nuovo muro nel vettore di muri del giocatore corrente
static void setWall(Coordinates centerPos, int direction){
	int i = ms.walls[ms.player-1].used;
	ms.walls[ms.player-1].position[i] = centerPos;
	ms.walls[ms.player-1].dir[i] = direction;
}

// Disegno di tutti i muri sulla scacchiera:
// utile in caso di muri pendenti non confermati, poi cancellati
static void redrawWalls(){
	int i, j;
	for(i = 0; i < 2; i++){
		for(j = 0; j < ms.walls[i].used; j++){
			drawWall(ms.walls[i].position[j], ms.walls[i].dir[j], WALL_COLORS[i]);
		}
	}
}

// Get nuova posizione, data la corrente e lo spostamento
static Coordinates getMovedPos(Coordinates currentPos, int h, int v){
	Coordinates newPos;
	
	newPos.x = currentPos.x + h;
	newPos.y = currentPos.y + v;
	
	return newPos;
}

// Spostamento effettivo di un muro, con colorazione nuovi pixel e
// ripristino di quelli vecchi
static void moveWall(int h, int v){
	Coordinates pos;
	int dir;
	
	// Ripristino sfondo sotto al vecchio muro
	pos = ms.walls[ms.player - 1].position[ms.walls[ms.player-1].used];
	dir = ms.walls[ms.player - 1].dir[ms.walls[ms.player-1].used];
	drawWall(pos, dir, BGCOLOR);
	
	// Ridisegno muri esistenti già presenti (necessario per sovrapposizioni)
	redrawWalls();
	
	// Disegno nuovo muro, in posizione spostata
	drawWall(getMovedPos(pos, h, v), dir, WALL_COLORS[ms.player-1]);
	
	// Aggiunta muro al vettore
	setWall(getMovedPos(pos, h, v), dir);
}

// Inizializzazione dati dei giocatori
static void initPlayers(){
	
	// Posizione default
	ms.currentPos[PLAYER1-1] = START_POS_PLAYER1;
	ms.currentPos[PLAYER2-1] = START_POS_PLAYER2;
	
	// Disegno pedine
	drawToken(START_POS_PLAYER1, PLAYER1_COLOR);
	drawToken(START_POS_PLAYER2, PLAYER2_COLOR);
	
	// Stampa muri iniziali
	writeWallsStats(MAX_NUM_WALLS, MAX_NUM_WALLS);
}

// Eliminazione muri
static void clearWalls(){
	ms.walls[PLAYER1-1].used = 0;
	ms.walls[PLAYER2-1].used = 0;
}

// Inizializzazione interfaccia
static void initInterface(){
	
	// Sfondo
	LCD_Clear(BGCOLOR);
	
	// Scacchiera e box per statistiche
	drawChessPlatform();
	
	// Nessun muro
	clearWalls();
}

// Inizializzazione logica di gioco
static void initPlayersData(){
	
	// Modalità di attesa
	setMode(WAITING);
	
	// Dati dei giocatori
	initPlayers();
	
	// Nessun muro in fase di posizionamento
	ms.pendingWall = 0;
	
	// Mantenimento scritta vincitore, se presente
	if(strlen(message) > 0){
		writeMessage(message);
	}
}

// Inizializzazione pulsanti
static void initControls(){
	// Abilitazione INT0
	enable_button(INT0_PIN, EINT0_IRQn);
	
	// Disabilitazione KEY1 e KEY2
	disable_button(KEY1_PIN, EINT1_IRQn);
	disable_button(KEY2_PIN, EINT2_IRQn);
}

// Salvataggio mossa nel formato richiesto
void saveMove(int playerId, int moveType, int wallOrientation, Coordinates *destPos){
	int newMove = 0;
	newMove |= (playerId << 24);
	newMove |= (moveType << 20);
	newMove |= (wallOrientation << 16);
	newMove |= (destPos->y << 8);
	newMove |= (destPos->x << 0);
	
	ms.lastMove = newMove;
}

// Inizializzazione gioco
void initGame(){
	initControls();
	initInterface();
	initPlayersData();
}

// Impostazione modalità di gioco
void setMode(int modeValue){
	
	ms.mode = modeValue;
	
	// Tempo iniziale
	if(ms.mode == PLAYING){
		ms.timeRemaining = 20;
	}
}

// Impostazione giocatore
void setPlayer(int playerValue){
	
	// Cancellazione tempo iniziale
	writeTimeRemaining(ms.timeRemaining, BGCOLOR);
	
	// In presenza di muri non confermati dal giocatore precedente
	// si ridisegnano quelli presenti (necessario in caso di sovrapposizioni)
	if(ms.pendingWall == 1){
		redrawWalls();
	}
	
	// Parametri iniziali
	ms.player = playerValue;
	ms.timeRemaining = 20;
	ms.pendingWall = 0;
	ms.lastMove = 0xFFFFFFFF;
	
	ms.validMove = 0;
	nextPos = ms.currentPos[ms.player-1];
	
	// Celle disponibili per spostamento
	setColorMove(ms.currentPos[ms.player-1], VALID_MOVE_COLOR);
	
	// Eliminazione messaggio
	clearMessage();
	
	// Scrittura tempo rimanente
	writeTimeRemaining(ms.timeRemaining, TIME_COLOR);
	
	// Avvio timer (1 s)
	enable_timer(0);
}

// Colorazione celle adiacenti per spostamento
void setColorMove(Coordinates pos, int color){
	int i;
	Coordinates tempPos, finalPos;
	
	for(i = 0; i < 4; i++){
		tempPos.x = pos.x + moves[i][0];
		tempPos.y = pos.y + moves[i][1];
		
		// Get cella dietro all'avversario, se adiacente
		jumpOverOpponent(&pos, &tempPos, ms.player, &finalPos);
		
		// Validazione posizione finale
		if(validPos(finalPos, ms.currentPos[ms.player-1])){
			
			// Check direzione non diagonale
			if(finalPos.x == pos.x ^ finalPos.y == pos.y){
				drawSquareArea(finalPos, color);
			}
		}
	}
}

// Check posizione uguale
int equalCoordinates(Coordinates a, Coordinates b){
	return a.x == b.x && a.y == b.y;
}

// Get nuova posizione pedina
Coordinates getNextPos(){
	return nextPos;
}

// Impostazione nuova posizione
void setNextPos(int h, int v){
	Coordinates finalPos, destPos;
	
	// Eliminazione messaggio, se presente
	clearMessage();
	
	// Get cella dietro all'avversario, se adiacente
	destPos = getMovedPos(ms.currentPos[ms.player - 1], h, v);
	jumpOverOpponent(&(ms.currentPos[ms.player-1]), &destPos, ms.player, &finalPos);
	
	// Validazione posizione finale
	ms.validMove = validPos(finalPos, ms.currentPos[ms.player-1]);
	if(ms.validMove){
		
		// Caso positivo: impostazione posizione
		nextPos = finalPos;
	}
	else{
		
		// Caso negativo: posizione invariata
		nextPos = ms.currentPos[ms.player-1];
	}
}

// Movimento pedina
void move(){
	
	// Posizione finale valida
	if(ms.validMove){
		
		// Stop timer
		reset_timer(0);
		
		// Cancellazione token
		drawToken(ms.currentPos[ms.player-1], BGCOLOR);
		
		// Cancellazione celle evidenziate adiacenti
		setColorMove(ms.currentPos[ms.player-1], BGCOLOR);
		
		// Disegno pedina in posizione finale
		drawToken(nextPos, PLAYER_COLORS[ms.player-1]);
		
		// Aggiornamento posizione finale
		ms.currentPos[ms.player-1] = nextPos;
		
		// Salvataggio mossa
		saveMove(ms.player-1, PLAYER_MOVE, PLAYER_MOVE, &(ms.currentPos[ms.player-1]));
		
		// Vittoria giocatore corrente
		// Impostazione messaggio vittoria
		// Inizializzazione gioco
		if(victory(ms.currentPos[ms.player-1], ms.player)){
			char pchar = (char)(ms.player + '0');
			char messageLoc[MESSAGE_LENGTH] = "Player ";
			strcat(messageLoc, (const char *) &pchar);
			strcat(messageLoc, " won! INT0 to restart");
			strcpy(message, messageLoc);
			initGame();
		}
		// Se nessuno ha vinto, cambio giocatore
		else{
			setPlayer(getOtherPlayer(ms.player));
		}
	}
	// Posizione finale non valida: stampa messaggio
	else{
		writeMessage("Move not valid");
	}
}

// Creazione nuovo muro
void newWall(Coordinates centerPos, int direction){
	
	// Muro in attesa di azione
	ms.pendingWall = 1;
	
	// Cancellazione celle per spostamento
	setColorMove(ms.currentPos[ms.player-1], BGCOLOR);
	
	// Impostazione muro
	setWall(centerPos, direction);
	
	// Disegno muro
	drawWall(centerPos, direction, WALL_COLORS[ms.player-1]);
}

// Rotazione muro
void rotateWall(){
	Coordinates pos;
	int currDir, nextDir;
	
	// Cancellazione muro iniziale
	pos = ms.walls[ms.player - 1].position[ms.walls[ms.player-1].used];
	currDir = ms.walls[ms.player - 1].dir[ms.walls[ms.player-1].used];
	drawWall(pos, currDir, BGCOLOR);
	
	// Ridisegno muri già presenti
	redrawWalls();
	
	// Disegno muro ruotato e sua impostazione
	nextDir = 1 - currDir;
	drawWall(pos, nextDir, WALL_COLORS[ms.player-1]);
	setWall(pos, nextDir);
}

// Conferma muro
void confirmWall(){
	int i, overlapped_wall_index, player_index;
	
	// Se muro valido, conferma
	i = ms.walls[ms.player-1].used;
	ms.validMove = validWallPos(&(ms.walls[ms.player - 1].position[i]), ms.walls[ms.player-1].dir[i],
			&overlapped_wall_index, &player_index);
	if(ms.validMove){
		
		// Stop timer
		reset_timer(0);
		
		// Muro confermato
		ms.pendingWall = 0;
				
		// Incremento numero muri usati
		ms.walls[ms.player-1].used++;
			
		// Salvataggio mossa
		saveMove(ms.player-1, WALL_PLACEMENT, ms.walls[ms.player-1].dir[i],
				&(ms.walls[ms.player - 1].position[i]));
		
		// Aggiornamento numero muri disponibili per i giocatori
		writeWallsStats(MAX_NUM_WALLS - ms.walls[PLAYER1-1].used, MAX_NUM_WALLS - ms.walls[PLAYER2-1].used);
				
		// Cambio giocatore
		setPlayer(getOtherPlayer(ms.player));
	}
	// Se muro non valido, messaggio di errore
	else{
		writeMessage("Position not valid");
	}
}

// Annullamento muro
void undoWall(){
	int dir;
	Coordinates pos;
	
	// Muro annullato
	ms.pendingWall = 0;
	
	// Cancellazione muro
	pos = ms.walls[ms.player-1].position[ms.walls[ms.player-1].used];
	dir = ms.walls[ms.player-1].dir[ms.walls[ms.player-1].used];
	drawWall(pos, dir, BGCOLOR);
	
	// Ridisegno muri già presenti
	redrawWalls();
	
	// Evidenziazione celle valide per spostamento
	setColorMove(ms.currentPos[ms.player-1], VALID_MOVE_COLOR);
}

// Impostazione nuovo muro
void setNextWall(int h, int v){
	int i;
	Coordinates pos;
	
	// Cancellazione eventuale messaggio
	clearMessage();
	
	// Se il muro si può spostare senza uscire dalla scacchiera,
	// lo si sposta, altrimenti si stampa un messaggio di errore
	i = ms.walls[ms.player-1].used;
	pos = getMovedPos(ms.walls[ms.player-1].position[i], h, v);
	if(centerWallInPlatform(&pos)){
		moveWall(h, v);
	}
	else{
		writeMessage("Position not valid");
	}
}


