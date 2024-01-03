#include <string.h>
#include "quoridor.h"
#include "../button/button.h"
#include "../timer/timer.h"
#include "../queue/queue.h"
#include "../graphics/interface.h"

// Posizioni (x,y) iniziali dei giocatori
const Coordinates START_POS_PLAYER1 = {3,6};
const Coordinates START_POS_PLAYER2 = {3,0};

// Posizione iniziale muro generico
const Coordinates WALL_DEFAULT_POS = {3,2};

// Colore pedina per ogni giocatore
const int PLAYER_COLORS[2] = {White, Red};

// Colore muro per ogni giocatore
const int WALL_COLORS[2] = {White, Red};

// Spostamenti consentiti
const int moves[4][2] = {
	{-1, 0},		// LEFT
	{0, -1},		// UP
	{0, 1},			// DOWN
	{1, 0}			// RIGHT
};

// Variabile di stato del gioco
MatchType ms;

// Posizione futura della pedina
Coordinates nextPos;

// Messaggio stampato a video
char message[MESSAGE_LENGTH] = "\0\0";


/** FUNZIONI INTERNE DEL GIOCO
 * Non sono esportate all'esterno
 * Servono solo alle funzioni principali per svolgere check,
 * get, set e operazioni complesse
 */


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

// Get nuova posizione pedina
Coordinates getNextPos(){
	return nextPos;
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
	
	// Calcolo step iniziali
	stepX = coordDiffX(*destPos, *srcPos);
	stepY = coordDiffY(*destPos, *srcPos);
	
	// In presenza di un avversario nella posizione destinazione,
	// si incrementa di 1 il modulo dello step, nella direzione opportuna
	if(equalCoord(ms.currentPos[getOtherPlayer(player) - 1], *destPos)){
		stepX = (equalCoordY(*srcPos, *destPos)) ? sign(stepX) * (abs(stepX) + 1) : 0;
		stepY = (equalCoordX(*srcPos, *destPos)) ? sign(stepY) * (abs(stepY) + 1) : 0;
	}
				
	// Assegnazione destinazione finale
	*finalPos = changeCoord(*srcPos, stepX, stepY);
}

// Check presenza di un muro specifico tra due celle
static int wallBetweenCells(Coordinates *srcPos, Coordinates *destPos, Coordinates *centerPos, int dir){
	
	// Muro orizzontale
	if(dir == HORIZONTAL_WALL){
		
		// Spostamento verticale verso il basso
		if(equalCoordX(*srcPos, *destPos) && (equalCoordX(*centerPos, *destPos) || equalCoordX(*destPos, changeCoord(*centerPos, 1, 0)))
			&& coordCmpY(*srcPos, *centerPos) <= 0 && coordCmpY(*centerPos, *destPos) < 0)
				return 1;
		// Spostamento verticale verso l'alto
		if(equalCoordX(*srcPos, *destPos) && (equalCoordX(*centerPos, *destPos) || equalCoordX(*destPos, changeCoord(*centerPos, 1, 0)))
			&& coordCmpY(*destPos, *centerPos) <= 0 && coordCmpY(*centerPos, *srcPos) < 0)
				return 1;
	}
	
	// Muro verticale
	else{
		
		// Spostamento verticale verso destra
		if(equalCoordY(*srcPos, *destPos) && (equalCoordY(*centerPos, *destPos) || equalCoordY(*destPos, changeCoord(*centerPos, 0, 1)))
			&& coordCmpX(*srcPos, *centerPos) <= 0 && coordCmpX(*centerPos, *destPos) < 0)
				return 1;
		// Spostamento verticale verso sinistra
		if(equalCoordY(*srcPos, *destPos) && (equalCoordY(*centerPos, *destPos) || equalCoordY(*destPos, changeCoord(*centerPos, 0, 1)))
			&& coordCmpX(*destPos, *centerPos) <= 0 && coordCmpX(*centerPos, *srcPos) < 0)
				return 1;
	}
	return 0;
}

// Check presenza di almeno un muro tra due celle
static int noWallBetween(Coordinates adjPos, Coordinates currentPos){
	int p, i, wall_in_progress;
	
	// Check su entrambi i giocatori
	for(p = 1; p <= 2; p++){
		
		// Check necessità controllo del muro in corso di posizionamento
		wall_in_progress = (p == ms.player && ms.pendingWall == 1);
		
		// Iterazione sui muri: terminazione anticipata con 0, se
		// un muro è posto tra le due celle
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
// una cella del lato opposto al proprio lato di partenza
// Si usa ricerca in ampiezza (BFS) di almeno un percorso esistente
// Le celle visitate sono inserite in una coda FIFO e marcate con un
// flag al momento dell'inserimento, per risparmiare spazio allocato
static int checkReachability(int player){
	
	int i;
	
	// Coda FIFO implementata con buffer circolare
	// per avere QUEUE_DIM < NUM_SQUARES^2 = 49
	Coordinates queue[QUEUE_DIM];
	
	// Vettore di flag, realizzato mediante variabile intera a 64 bit
	// Il bit k-esimo corrisponde alla cella (x,y), dove
	// k = x * N + y, con N pari al numero di celle per lato della griglia
	unsigned long long enqueued;
	
	// Variabili di tipo (x,y) per agire sulle celle
	Coordinates currElem, adjElem, finalPos;
	
	// Azzeramento flag di visita
	enqueued = 0;
	
	// Inizializzazione coda FIFO e
	// inserimento posizione corrente del giocatore
	initQueue(queue);
	currElem = ms.currentPos[player-1];
	enqueue(queue, currElem);
	enqueued |= (1ULL << (currElem.x * NUM_SQUARES + currElem.y));  
	
	// La ricerca procede, nel caso negativo,
	// fino allo svuotamento della coda
	while(!isEmpty(queue)){
		currElem = dequeue(queue);
		
		// Check vittoria
		if(victory(currElem, player)){
			return 1;
		}
		
		// Iterazione sulle adiacenze
		for(i = 0; i < 4; i++){
			
			// Set adiacenza corrente
			adjElem = changeCoord(currElem, moves[i][0], moves[i][1]);
			
			// Eventuale correzione in presenza avversario
			// Considera l'avversario come un ostacolo
			// Determinante se esso è chiuso solo posteriormente da un muro
			jumpOverOpponent(&currElem, &adjElem, player, &finalPos);
			
			// Validazione posizione
			if(validPos(finalPos, currElem)){
				
				// Se la cella non è in coda, si inserisce e si marca il flag
				if((enqueued & (1ULL << (finalPos.x * NUM_SQUARES + finalPos.y))) == 0){
					enqueue(queue, finalPos);
					enqueued |= (1ULL << (finalPos.x * NUM_SQUARES + finalPos.y));
				}
			}
		}
	}
	
	return 0;
}

// Check esistenza di un muro di un giocatore specifico sovrapposto a quello corrente
// Si salva l'indice del muro a cui il corrente si sovrappone
// Ritorna un flag boolano, che indica sovrapposizione
static int exists_overlapping_wall(int playerWalls, Coordinates *centerPos, int dir){
	int i, numWalls, overlap;
	
	// Numero muri del giocatore da verificare
	numWalls = ms.walls[playerWalls-1].used;
	for(i = overlap = 0; i < numWalls && !overlap; i++){
		
		// Stessa direzione: sovrapposti se il centro dei muri
		// differisce per al più un'unità ( = una cella) 
		if(dir == ms.walls[playerWalls-1].dir[i]){
			if(dir == HORIZONTAL_WALL){
				overlap = equalCoordY(ms.walls[playerWalls-1].position[i], *centerPos) &&
									coordDistX(ms.walls[playerWalls-1].position[i], *centerPos) <= 1;
			}
			else{
				overlap = equalCoordX(ms.walls[playerWalls-1].position[i], *centerPos) && 
									coordDistY(ms.walls[playerWalls-1].position[i], *centerPos) <= 1;
			}
		}
		// Direzione ortogonale: sovrapposti solo se aventi stesso centro
		else{
			overlap = equalCoord(ms.walls[playerWalls-1].position[i], *centerPos);
		}
	} 
	
	return overlap;
}

// Check esistenza di un muro sovrapposto a quello corrente
// Si salvano l'indice del muro a cui il corrente si sovrappone e
// l'id del giocatore {1,2} a cui appartiene il muro
// Ritorna un flag booleano di non sovrapposizione
static int checkNotOverlapping(Coordinates *centerPos, int dir){
	int overlap1, overlap2;
	
	// Check sovrapposizione su entrambi i giocatori
	overlap1 = exists_overlapping_wall(PLAYER1, centerPos, dir);
	overlap2 = exists_overlapping_wall(PLAYER2, centerPos, dir);
	
	return !(overlap1 || overlap2);
}

// Check validazione muro
static int validWallPos(Coordinates *centerPos, int dir){
	int r1, r2, not_overlap, valid_position;
	
	// Non out-of-range
	valid_position = centerWallInPlatform(centerPos);
	
	// Non impedisce possibilità di vittoria per entrambi i giocatori
	r1 = checkReachability(PLAYER1);
	r2 = checkReachability(PLAYER2);
	
	// Non si sovrappone a muri esistenti
	not_overlap = checkNotOverlapping(centerPos, dir);
	
	return valid_position && r1 && r2 && not_overlap;
}

// Inserimento del nuovo muro nel vettore di muri del giocatore corrente
static void setWall(Coordinates centerPos, int direction){
	int i;
	
	i = ms.walls[ms.player-1].used;
	ms.walls[ms.player-1].position[i] = centerPos;
	ms.walls[ms.player-1].dir[i] = direction;
}

// Disegno di tutti i muri sulla scacchiera:
// utile in caso di muri pendenti non confermati, poi cancellati
static void redrawWalls(){
	int i, j;
	for(i = 0; i <= 1; i++){
		for(j = 0; j < ms.walls[i].used; j++){
			drawWall(ms.walls[i].position[j].x, ms.walls[i].position[j].y,
				ms.walls[i].dir[j], WALL_COLORS[i]);
		}
	}
}

// Spostamento effettivo di un muro, con colorazione nuovi pixel e
// ripristino di quelli vecchi
static void moveWall(int h, int v){
	Coordinates pos;
	int dir;
	
	// Ripristino sfondo sotto al vecchio muro
	pos = ms.walls[ms.player - 1].position[ms.walls[ms.player - 1].used];
	dir = ms.walls[ms.player - 1].dir[ms.walls[ms.player - 1].used];
	drawWall(pos.x, pos.y, dir, BGCOLOR);
	
	// Ridisegno muri esistenti già presenti (necessario per sovrapposizioni)
	redrawWalls();
	
	// Disegno nuovo muro, in posizione spostata
	pos = changeCoord(pos, h, v);
	drawWall(pos.x, pos.y, dir, WALL_COLORS[ms.player-1]);
	
	// Aggiunta muro al vettore
	setWall(pos, dir);
}

// Set messaggio vittoria
static void setVictoryMessage(){
	char pchar = (char)(ms.player + '0');
	char messageLoc[MESSAGE_LENGTH] = "Player ";
	strcat(messageLoc, (const char *) &pchar);
	strcat(messageLoc, " won! INT0 to restart");
	strcpy(message, messageLoc);
}

// Inizializzazione dati dei giocatori
static void initPlayers(){
	
	// Posizione default
	ms.currentPos[PLAYER1-1] = START_POS_PLAYER1;
	ms.currentPos[PLAYER2-1] = START_POS_PLAYER2;
	
	// Disegno pedine
	drawToken(START_POS_PLAYER1.x, START_POS_PLAYER1.y, PLAYER1_COLOR);
	drawToken(START_POS_PLAYER2.x, START_POS_PLAYER2.y, PLAYER2_COLOR);
	
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


/* FUNZIONI PRINCIPALI DEL GIOCO
 * Sono invocate in altri file (handlers e main)
 * Svolgono le principali funzioni richieste dal gioco
*/


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
	
	// Cancellazione messaggio
	clearMessage();
	
	// In presenza di muri non confermati dal giocatore precedente
	// si ridisegnano quelli presenti (necessario in caso di sovrapposizioni)
	if(ms.pendingWall == 1){
		redrawWalls();
	}
	
	// Set parametri iniziali
	ms.player = playerValue;
	ms.timeRemaining = 20;
	ms.pendingWall = 0;
	ms.lastMove = 0xFFFFFFFF;
	
	ms.validMove = 0;
	nextPos = ms.currentPos[ms.player-1];
	
	// Celle disponibili per spostamento
	setColorMove(ms.currentPos[ms.player-1], VALID_MOVE_COLOR);
	
	// Scrittura tempo rimanente
	writeTimeRemaining(ms.timeRemaining, TIME_COLOR);
	
	// Avvio timer (1 s)
	enable_timer(0);
}

// Colorazione celle adiacenti per spostamento
void setColorMove(Coordinates pos, int color){
	int i;
	Coordinates tempPos, finalPos;
	
	// Iterazioni sulle adiacenze
	for(i = 0; i < 4; i++){
		
		// Get nuova adiacenza
		tempPos = changeCoord(pos, moves[i][0], moves[i][1]);
		
		// Get cella dietro all'avversario, se adiacente
		jumpOverOpponent(&pos, &tempPos, ms.player, &finalPos);
		
		// Validazione posizione finale
		if(validPos(finalPos, ms.currentPos[ms.player-1])){
			
			// Check direzione non diagonale
			if(finalPos.x == pos.x ^ finalPos.y == pos.y){
				drawSquareArea(finalPos.x, finalPos.y, color);
			}
		}
	}
}

// Impostazione nuova posizione
void setNextPos(int h, int v){
	Coordinates finalPos, destPos;
	
	// Eliminazione messaggio, se presente
	clearMessage();
	
	// Get cella dietro all'avversario, se adiacente
	destPos = changeCoord(ms.currentPos[ms.player - 1], h, v);
	jumpOverOpponent(&(ms.currentPos[ms.player-1]), &destPos, ms.player, &finalPos);
	
	// Validazione posizione finale
	ms.validMove = validPos(finalPos, ms.currentPos[ms.player-1]);
	if(ms.validMove){
		
		// Caso positivo: impostazione posizione
		nextPos = finalPos;
	}
	else{
		
		// Caso negativo: posizione invariata e stampa messaggio errore
		nextPos = ms.currentPos[ms.player-1];
		writeMessage("Move not valid");
	}
}

// Conferma il movimento della pedina
void move(){
	
	// Posizione finale valida
	if(ms.validMove){
		
		// Stop timer
		reset_timer(0);
		
		// Cancellazione token
		drawToken(ms.currentPos[ms.player-1].x, ms.currentPos[ms.player-1].y, BGCOLOR);
		
		// Cancellazione celle evidenziate adiacenti
		setColorMove(ms.currentPos[ms.player-1], BGCOLOR);
		
		// Disegno pedina in posizione finale
		drawToken(nextPos.x, nextPos.y, PLAYER_COLORS[ms.player-1]);
		
		// Aggiornamento posizione finale
		ms.currentPos[ms.player-1] = nextPos;
		
		// Salvataggio mossa
		saveMove(ms.player-1, PLAYER_MOVE, PLAYER_MOVE, &(ms.currentPos[ms.player-1]));
		
		// Vittoria giocatore corrente
		// Impostazione messaggio vittoria
		// Inizializzazione gioco
		if(victory(ms.currentPos[ms.player-1], ms.player)){
			setVictoryMessage();
			initGame();
		}
		// Se nessuno ha vinto, cambio giocatore
		else{
			setPlayer(getOtherPlayer(ms.player));
		}
	}
}

// Creazione nuovo muro
void newWall(Coordinates centerPos, int direction){
	
	// Muro in attesa di azione
	ms.pendingWall = 1;
	
	// Ripristino posizione pedina, per annullamento movimento
	nextPos = ms.currentPos[ms.player-1];
	
	// Cancellazione celle per spostamento
	setColorMove(ms.currentPos[ms.player-1], BGCOLOR);
	
	// Impostazione muro
	setWall(centerPos, direction);
	
	// Disegno muro
	drawWall(centerPos.x, centerPos.y, direction, WALL_COLORS[ms.player-1]);
}

// Rotazione muro
void rotateWall(){
	Coordinates pos;
	int currDir, nextDir;
	
	// Cancellazione messaggio
	clearMessage();
	
	// Cancellazione muro iniziale
	pos = ms.walls[ms.player - 1].position[ms.walls[ms.player-1].used];
	currDir = ms.walls[ms.player - 1].dir[ms.walls[ms.player-1].used];
	drawWall(pos.x, pos.y, currDir, BGCOLOR);
	
	// Ridisegno muri già presenti
	redrawWalls();
	
	// Disegno muro ruotato e sua impostazione
	nextDir = 1 - currDir;
	drawWall(pos.x, pos.y, nextDir, WALL_COLORS[ms.player-1]);
	setWall(pos, nextDir);
}

// Conferma muro
void confirmWall(){
	int i;
	
	// Check validazione muro
	i = ms.walls[ms.player-1].used;
	ms.validMove = validWallPos(&(ms.walls[ms.player - 1].position[i]), ms.walls[ms.player-1].dir[i]);
	
	// Se muro valido, conferma
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
	drawWall(pos.x, pos.y, dir, BGCOLOR);
	
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
	
	// Get posizione spostata del muro
	i = ms.walls[ms.player-1].used;
	pos = changeCoord(ms.walls[ms.player-1].position[i], h, v);
	
	// Se il muro si può spostare senza uscire dalla scacchiera,
	// lo si sposta, altrimenti si stampa un messaggio di errore
	if(centerWallInPlatform(&pos)){
		moveWall(h, v);
	}
	else{
		writeMessage("Position not valid");
	}
}

// Salvataggio mossa nel formato richiesto
void saveMove(int playerId, int moveType, int wallOrientation, Coordinates *destPos){
	int newMove; 
	
	newMove = 0	| (playerId << 24) 
							| (moveType << 20)
							| (wallOrientation << 16)
							| (destPos->y << 8)
							| (destPos->x << 0);
	
	ms.lastMove = newMove;
}
