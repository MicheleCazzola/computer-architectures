#include <string.h>
#include "quoridor.h"
#include "../button/button.h"
#include "../timer/timer.h"
#include "../queue/queue.h"
#include "../graphics/interface.h"
#include "../CAN/CAN.h"

// Messaggi per menu
const char MENU_MESSAGES[2][4][30] = {
	{MENU2_1_Q1, MENU2_1_Q2, MENU2_OPT1, MENU2_OPT2},
	{MENU2_2_Q1, MENU2_2_Q2, MENU2_OPT1, MENU2_OPT2}
};

// Posizioni (x,y) iniziali dei giocatori
const Coordinates START_POS_PLAYER1 = {NUM_SQUARES/2,NUM_SQUARES-1};
const Coordinates START_POS_PLAYER2 = {NUM_SQUARES/2,0};

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
	{1, 0},			// RIGHT
	{0, 1}			// DOWN
};

// Variabile per modalità di gioco
ModeType gm;

// Variabile di stato del gioco
MatchType ms;

// Scelta futura nel menu
int provChoice;

// Posizione futura della pedina
Coordinates nextPos;

// Messaggio stampato a video
char message[MESSAGE_LENGTH] = "\0\0";


/** FUNZIONI INTERNE DEL GIOCO
 * Non sono esportate all'esterno
 * Servono solo alle funzioni principali per svolgere check,
 * get, set e operazioni complesse
 */



// Check posizione dentro ai bordi
static int posInPlatform(Coordinates pos){
	return 0 <= pos.x && pos.x < NUM_SQUARES && 0 <= pos.y && pos.y < NUM_SQUARES;
}

// Check muro dentro ai bordi
static int centerWallInPlatform(Coordinates pos){
	return 0 <= pos.x && pos.x < NUM_SQUARES-1 && 0 <= pos.y && pos.y < NUM_SQUARES-1;
}

// Check presenza di un muro specifico tra due celle
static int wallBetweenCells(Coordinates *srcPos, Coordinates *destPos, Coordinates *centerPos, char dir){
	
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
		
		// Spostamento orizzontale verso destra
		if(equalCoordY(*srcPos, *destPos) && (equalCoordY(*centerPos, *destPos) || equalCoordY(*destPos, changeCoord(*centerPos, 0, 1)))
			&& coordCmpX(*srcPos, *centerPos) <= 0 && coordCmpX(*centerPos, *destPos) < 0)
				return 1;
		// Spostamento orizzontale verso sinistra
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
	for(p = 0; p <= 1; p++){
		
		// Check necessità controllo del muro in corso di posizionamento
		wall_in_progress = (p == ms.player && ms.pendingWall == 1);
		
		// Iterazione sui muri: terminazione anticipata con 0, se
		// un muro è posto tra le due celle
		for(i = 0; i < ms.walls[p].used + wall_in_progress; i++){
			if(wallBetweenCells(&currentPos, &adjPos, &(ms.walls[p].position[i]), ms.walls[p].dir[i])){
				return 0;
			}
		}
	}
	
	return 1;
}



// Get posizione finale in presenza di avversario in posizione adiacente a quella corrente
static int jumpOverOpponent(Coordinates *destPos, Coordinates *finalPos, int h, int v){
	
	// Assegnazione destinazione finale
	*finalPos = changeCoord(*destPos, h, v);
	
	return validPos(*finalPos, *destPos);
}


// Verifica se la posizione è di una cella evidenziata
static int isHighlitedAdj(Coordinates *pos, Coordinates *finalPos, int h, int v){
	int i;
	Coordinates behindPos;
	for(i = 0; i < ms.numHighlited; i++){
		
		// Se non bisogna scavalcare l'avversario
		if(!equalCoord(*pos, ms.currentPos[getOtherPlayer(ms.player)])){
			
			// Se l'adiacenza è selezionata -> Si assegna come posizione finale
			if(equalCoord(ms.highlited[i], *pos)){
				*finalPos = *pos;
				return 1;
			}
		}
		
		// Se bisogna scavalcare l'avversario
		else{
			// Se la posizione dietro l'avversario è valida
			if(jumpOverOpponent(pos, &behindPos, h, v)){
				if(equalCoord(ms.highlited[i], behindPos)){
					*finalPos = behindPos;
					return 1;
				}
			}
		}
	}
	
	return 0;
}

// Check raggiungibilità, da parte di un giocatore, di almeno
// una cella del lato opposto al proprio lato di partenza
// Si usa ricerca in ampiezza (BFS) di almeno un percorso esistente
// Le celle visitate sono inserite in una coda FIFO e marcate con un
// flag al momento dell'inserimento, per risparmiare spazio allocato
static int checkReachability(char player){
	
	int i;
	
	// Coda FIFO implementata con buffer circolare
	// per avere QUEUE_DIM < NUM_SQUARES^2 = 49
	Coordinates queue[QUEUE_DIM];
	
	// Vettore di flag, realizzato mediante variabile intera a 64 bit
	// Il bit k-esimo corrisponde alla cella (x,y), dove
	// k = x * N + y, con N pari al numero di celle per lato della griglia
	unsigned long long enqueued;
	
	// Variabili di tipo (x,y) per agire sulle celle
	Coordinates currElem, adjElem;
	
	// Azzeramento flag di visita
	enqueued = 0;
	
	// Inizializzazione coda FIFO e
	// inserimento posizione corrente del giocatore
	initQueue(queue);
	currElem = ms.currentPos[player];
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
		
		// Check raggiungibilità destinazione, senza considerare
		// la posizione corrente dell'avversario
		// Iterazione sulle celle adiacenti
		for(i = 0; i < 4; i++){
			
			// Set adiacenza corrente
			adjElem = changeCoord(currElem, moves[i][0], moves[i][1]);
			
			if(validPos(adjElem, currElem)){
				// Se la cella non è in coda, si inserisce e si marca il flag
				if((enqueued & (1ULL << (adjElem.x * NUM_SQUARES + adjElem.y))) == 0){
					enqueue(queue, adjElem);
					enqueued |= (1ULL << (adjElem.x * NUM_SQUARES + adjElem.y));
				}
			}
		}
	}
	
	return 0;
}

// Check esistenza di un muro di un giocatore specifico sovrapposto a quello corrente
// Si salva l'indice del muro a cui il corrente si sovrappone
// Ritorna un flag boolano, che indica sovrapposizione
static int exists_overlapping_wall(char playerWalls, Coordinates centerPos, char dir){
	int i, numWalls, overlap;
	
	// Numero muri del giocatore da verificare
	numWalls = ms.walls[playerWalls].used;
	for(i = overlap = 0; i < numWalls && !overlap; i++){
		
		// Stessa direzione: sovrapposti se il centro dei muri
		// differisce per al più un'unità ( = una cella) 
		if(dir == ms.walls[playerWalls].dir[i]){
			if(dir == HORIZONTAL_WALL){
				overlap = equalCoordY(ms.walls[playerWalls].position[i], centerPos) &&
									coordDistX(ms.walls[playerWalls].position[i], centerPos) <= 1;
			}
			else{
				overlap = equalCoordX(ms.walls[playerWalls].position[i], centerPos) && 
									coordDistY(ms.walls[playerWalls].position[i], centerPos) <= 1;
			}
		}
		// Direzione ortogonale: sovrapposti solo se aventi stesso centro
		else{
			overlap = equalCoord(ms.walls[playerWalls].position[i], centerPos);
		}
	} 
	
	return overlap;
}


// Check validazione muro
static int validWallPos(Coordinates centerPos, char dir){
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
	pos = ms.walls[ms.player].position[ms.walls[ms.player].used];
	dir = ms.walls[ms.player].dir[ms.walls[ms.player].used];
	drawWall(pos.x, pos.y, dir, BGCOLOR);
	
	// Ridisegno muri esistenti già presenti (necessario per sovrapposizioni)
	redrawWalls();
	
	// Disegno nuovo muro, in posizione spostata
	pos = changeCoord(pos, h, v);
	drawWall(pos.x, pos.y, dir, WALL_COLORS[ms.player]);
	
	// Aggiunta muro al vettore
	setWall(pos, dir, ms.player);
}

// Inizializzazione dati dei giocatori
static void initPlayers(){
	
	// Posizione default
	ms.currentPos[PLAYER1] = START_POS_PLAYER1;
	ms.currentPos[PLAYER2] = START_POS_PLAYER2;
	
	// Disegno pedine
	drawToken(START_POS_PLAYER1.x, START_POS_PLAYER1.y, PLAYER1_COLOR);
	drawToken(START_POS_PLAYER2.x, START_POS_PLAYER2.y, PLAYER2_COLOR);
	
	// Stampa muri iniziali
	writeWallsStats(MAX_NUM_WALLS, MAX_NUM_WALLS);
	
	// Inizializzazione flag NPC
	ms.finishedNPCMove = 0;
}

// Eliminazione muri
static void clearWalls(){
	ms.walls[PLAYER1].used = 0;
	ms.walls[PLAYER2].used = 0;
}

// Inizializzazione interfaccia
static void initInterface(){
	
	// Sfondo
	LCD_Clear(BGCOLOR);
	
	// Scacchiera e box per statistiche
	drawChessPlatform();
	
	// Nessun muro
	clearWalls();
	
	// Disegno banda
	if(gm.numBoards == 2){
		drawPlayerColor(PLAYER_COLORS[gm.boardPlayer]);
	}
}

// Inizializzazione logica di gioco
static void initPlayersData(){
	
	// Modalità di attesa
	setMode(WAITING);
	
	// Dati dei giocatori
	initPlayers();
	
	// Nessun muro in fase di posizionamento
	ms.pendingWall = 0;
	
	// Inizialmente, nessuna cella evidenziata
	ms.numHighlited = 0;
	
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

// Inizializzazione dati scelta
static void initChoiceData(){
	
	gm.handshake = HANDSHAKE_OFF;
	
	gm.numBoards = 0;
	
	gm.playersType[0] = gm.playersType[1] = NO_PLAYER;
	
	provChoice = 0;
}


/* FUNZIONI PRINCIPALI DEL GIOCO
 * Sono invocate in altri file (handlers e main)
 * Svolgono le principali funzioni richieste dal gioco
*/



// Check vittoria
int victory(Coordinates pos, char player){
	return pos.y == (NUM_SQUARES-1) * player;
}

// Get numero muri liberi
int getAvailableWalls(char player){
	return MAX_NUM_WALLS - ms.walls[player].used;
}

// Check esistenza di un muro sovrapposto a quello corrente
// Si salvano l'indice del muro a cui il corrente si sovrappone e
// l'id del giocatore {1,2} a cui appartiene il muro
// Ritorna un flag booleano di non sovrapposizione
int checkNotOverlapping(Coordinates centerPos, char dir){
	int overlap1, overlap2;
	
	// Check sovrapposizione su entrambi i giocatori
	overlap1 = exists_overlapping_wall(PLAYER1, centerPos, dir);
	overlap2 = exists_overlapping_wall(PLAYER2, centerPos, dir);
	
	return !(overlap1 || overlap2);
}

// Check posizione valida per spostamento
int validPos(Coordinates destPos, Coordinates currentPos){
	int inPlatform, connected;
	
	// Non out-of-range
	inPlatform = posInPlatform(destPos);
	
	// Check su muri solo se posizione valida su scacchiera
	if(inPlatform){
		
		// Assenza di muri tra le celle interessate
		connected = noWallBetween(destPos, currentPos);
		
		return connected;
	}
	
	return 0;
}

// Selezione celle da evidenziare, tra quelle possibili
void selectAdj(Coordinates *pos, char player, Coordinates *selected, char *numSelected){
	int i;
	Coordinates adjPos, finalPos;
	
	// Iterazione sulle adiacenze
	for(i = 0; i < 4; ++i){
		adjPos = changeCoord(*pos, moves[i][0], moves[i][1]);
		
		// Se la posizione adiacente è valida, si procede
		if(validPos(adjPos, *pos)){
			
			// Se nell'adiacente è presente l'altro giocatore ->
			// Check su posizioni adiacenti ad esso
			if(equalCoord(adjPos, ms.currentPos[getOtherPlayer(player)])){
				
				// Se la cella dietro all'avversario è valida, si seleziona solo quella
				if(jumpOverOpponent(&adjPos, &finalPos, moves[i][0], moves[i][1])){
					selected[(*numSelected)++] = finalPos;
				}
				
				// Altrimenti, si effettua lo stesso check sulle posizioni laterali
				// rispetto all'avversario, eventualmente selezionandole entrambe
				else{
					
					// Senso orario rispetto alla posizione dietro all'avversario
					if(jumpOverOpponent(&adjPos, &finalPos, moves[(i+1) % 4][0], moves[(i+1) % 4][1])){
						selected[(*numSelected)++] = finalPos;
					}
					
					// Senso antiorario rispetto alla posizione dietro all'avversario
					if(jumpOverOpponent(&adjPos, &finalPos, moves[(i + 4 -1) % 4][0], moves[(i + 4 - 1) % 4][1])){
						selected[(*numSelected)++] = finalPos;
					}
				}
			}
			// Altrimenti -> Si seleziona l'adiacente corrente
			else{
				selected[(*numSelected)++] = adjPos;
			}
		}
	}
}

// Set messaggio vittoria
void setVictoryMessage(){
	char pchar = (char)(ms.player + 1 + '0');
	char messageLoc[MESSAGE_LENGTH];
	
	if(gm.numBoards == 1){
		strcpy(messageLoc, "Player ");
		strcat(messageLoc, (const char *) &pchar);
	}
	else{
		if(gm.boardPlayer == ms.player){
			strcpy(messageLoc, "You");
		}
		else{
			strcpy(messageLoc, "Opponent");
		}
	}
	
	strcat(messageLoc, " won! INT0 to restart");
	strcpy(message, messageLoc);
}

// Inserimento del nuovo muro nel vettore di muri del giocatore corrente
void setWall(Coordinates centerPos, char direction, char playerId){
	int i;
	
	i = ms.walls[playerId].used;
	ms.walls[playerId].position[i] = centerPos;
	ms.walls[playerId].dir[i] = direction;
}

// Inizializzazione gioco
void initGame(){
	
	// Inizializzazione pulsanti
	initControls();
	
	// Inizializzazione schermo
	initInterface();
	
	// Inizializzazione dati di gioco
	initPlayersData();
	
	// Inizializzazione dati scelta
	initChoiceData();
}

// Impostazione modalità di gioco
void setMode(char modeValue){
	
	ms.mode = modeValue;
	
	// Tempo iniziale
	if(ms.mode == PLAYING){
		ms.timeRemaining = 20;
	}
	// Scelta iniziale
	else if(ms.mode == WAITING){
		provChoice = DEFAULT_CHOICE;
	}
}

// Impostazione giocatore
void setPlayer(char playerValue){
	
	char other;
	
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
	ms.validMove = 0;
	ms.finishedNPCMove = 0;
	ms.lastMove = 0xFFFFFFFF;
	
	// Set posizione successiva uguale a corrente
	// Modificata solo in seguito a movimento lungo
	// una direzione (non diagonale)
	nextPos = ms.currentPos[ms.player];
	
	// Cancellazione evidenziazione cella e ridisegno pedina giocatore precedente
	other = getOtherPlayer(ms.player);
	drawSquareArea(ms.currentPos[other].x, ms.currentPos[other].y, BGCOLOR);
	drawToken(ms.currentPos[other].x, ms.currentPos[other].y, PLAYER_COLORS[other]);
	
	// Giocatore umano
	if(gm.playersType[ms.player] == HUMAN){
		
		// Evidenziazione cella del giocatore
		drawSquareArea(ms.currentPos[ms.player].x, ms.currentPos[ms.player].y, TOKEN_BGCOLOR);
		
		// Ridisegno pedina
		drawToken(ms.currentPos[ms.player].x, ms.currentPos[ms.player].y, PLAYER_COLORS[ms.player]);
		
		// Celle disponibili per spostamento
		highliteAdj(ms.currentPos[ms.player]);
		
		// Scrittura tempo rimanente
		writeTimeRemaining(ms.timeRemaining, TIME_COLOR);
		
		// Avvio timer (1 s)
		enable_timer(0);
	}
	// Giocatore NPC: si suppone che impieghi meno di 20 secondi a giocare (su scheda impiega meno di 1 s)
	else{
		
		// Azzeramento flag mossa NPC conclusa
		ms.finishedNPCMove = 0;
		
		// Chiamata a funzione che effettua la mossa "migliore"
		NPC_playTurn(&ms, &gm, &nextPos);
	}
}

// Get avversario
int getOtherPlayer(char player){
	return 1 - player;
}

// Evidenziazione celle adiacenti per spostamento
void highliteAdj(Coordinates pos){
	int i;
	
	// Selezione adiacenze valide
	selectAdj(&pos, ms.player, ms.highlited, &ms.numHighlited);
	
	for(i = 0; i < ms.numHighlited; i++){
		drawSquareArea(ms.highlited[i].x, ms.highlited[i].y, VALID_MOVE_COLOR);
	}
}

// Cancellazione celle evidenziate
void eraseHighlightedAdj(){
	int i;
	
	// Iterazione su celle evidenziate
	for(i = 0; i < ms.numHighlited; i++){
		
		// Disegno area quadrato con bgcolor
		drawSquareArea(ms.highlited[i].x, ms.highlited[i].y, BGCOLOR);
	}
	
	// Azzeramento contatore celle evidenziate
	ms.numHighlited = 0;
}

// Impostazione nuova posizione
void setNextPos(int h, int v){
	Coordinates destPos, finalPos;
	
	// Eliminazione messaggio, se presente
	clearMessage();
	
	// Se mossa precedente esistente, la si cancella
	if(!equalCoord(ms.currentPos[ms.player], nextPos)){
		drawTokenBorder(nextPos.x, nextPos.y, VALID_MOVE_COLOR);
	}
	
	// Check se posizione è di una cella evidenziata (ovvero valida)
	destPos = changeCoord(ms.currentPos[ms.player], h, v);
	ms.validMove = isHighlitedAdj(&destPos, &finalPos, h, v);
	if(ms.validMove){
		
		// Caso positivo: impostazione posizione
		nextPos = finalPos;
		
		// Disegno bordo token
		drawTokenBorder(nextPos.x, nextPos.y, TOKEN_BORDER_COLOR);
	}
	else{
		
		// Caso negativo: posizione invariata e stampa messaggio errore
		nextPos = ms.currentPos[ms.player];
		writeMessage("Move not valid");
	}
}

// Conferma il movimento della pedina
void move(){
	
	// Stop timer: no interruzioni dopo la conferma di movimento
	disable_timer(0);
	
	// Posizione finale valida -> Salvataggio dati e cambio giocatore
	if(ms.validMove){
		
		// Reset timer: si riporta a 0 se avviene la conferma
		reset_timer(0);
		
		// Cancellazione token
		drawSquareArea(ms.currentPos[ms.player].x, ms.currentPos[ms.player].y, BGCOLOR);
		
		// Cancellazione posizioni evidenziate
		eraseHighlightedAdj();
		
		// Disegno pedina in posizione finale
		drawToken(nextPos.x, nextPos.y, PLAYER_COLORS[ms.player]);
		
		// Aggiornamento posizione finale
		ms.currentPos[ms.player] = nextPos;
		
		// Salvataggio ed invio mossa
		saveMove(ms.player, PLAYER_MOVE, PLAYER_MOVE, &(ms.currentPos[ms.player]));
		
		// Se multi-board, invio mossa anche in caso di vittoria
		if(gm.numBoards == 2){
			sendMove();
		}
		
		// Vittoria giocatore corrente
		// Impostazione messaggio vittoria
		// Inizializzazione gioco
		if(victory(ms.currentPos[ms.player], ms.player)){
			setVictoryMessage();
			initGame();
		}
		// Nessuno ha vinto
		else{
			
			// Single board -> Cambio giocatore
			if(gm.numBoards == 1){
				setPlayer(getOtherPlayer(ms.player));
			}
			// Double board -> Set giocatore avversario e attesa CAN
			else{
				setOpponentTurn();
			}
		}
	}
	else{
		
		// Si riabilita il timer: il gioco continua a proseguire
		// sul giocatore corrente
		enable_timer(0);
	}
}

// Creazione nuovo muro
void newWall(Coordinates centerPos, char direction){
	
	// Muro in attesa di azione
	ms.pendingWall = 1;
	
	// Ripristino posizione pedina, per annullamento movimento
	nextPos = ms.currentPos[ms.player];
	
	// Cancellazione posizioni evidenziate
	eraseHighlightedAdj();
	
	// Impostazione muro
	setWall(centerPos, direction, ms.player);
	
	// Disegno muro
	drawWall(centerPos.x, centerPos.y, direction, WALL_COLORS[ms.player]);
}

// Rotazione muro
void rotateWall(){
	
	Coordinates pos;
	int currDir, nextDir;
	
	// Cancellazione messaggio
	clearMessage();
	
	// Cancellazione muro iniziale
	pos = ms.walls[ms.player].position[ms.walls[ms.player].used];
	currDir = ms.walls[ms.player].dir[ms.walls[ms.player].used];
	drawWall(pos.x, pos.y, currDir, BGCOLOR);
	
	// Ridisegno muri già presenti
	redrawWalls();
	
	// Disegno muro ruotato e sua impostazione
	nextDir = 1 - currDir;
	drawWall(pos.x, pos.y, nextDir, WALL_COLORS[ms.player]);
	setWall(pos, nextDir, ms.player);
}

// Conferma muro
void confirmWall(){
	int i;
	
	// Stop timer: no interruzioni dopo la conferma di muro
	disable_timer(0);
	
	// Check validazione muro
	i = ms.walls[ms.player].used;
	ms.validMove = validWallPos(ms.walls[ms.player].position[i], ms.walls[ms.player].dir[i]);
	
	// Se muro valido -> conferma e cambio giocatore
	if(ms.validMove){
		
		// Reset timer: si riporta a 0
		reset_timer(0);
		
		// Muro confermato
		ms.pendingWall = 0;
				
		// Incremento numero muri usati
		ms.walls[ms.player].used++;
			
		// Salvataggio mossa
		saveMove(ms.player, WALL_PLACEMENT, ms.walls[ms.player].dir[i],
				&(ms.walls[ms.player].position[i]));
		
		// Aggiornamento numero muri disponibili per i giocatori
		writeWallsStats(getAvailableWalls(PLAYER1), getAvailableWalls(PLAYER2));
				
		// Single board -> Cambio giocatore
		if(gm.numBoards == 1){
			setPlayer(getOtherPlayer(ms.player));
		}
		// Double board
		else{
			
			// Invio mossa
			sendMove();
			
			// Set giocatore avversario e attesa CAN
			setOpponentTurn();
		}
		
	}
	// Se muro non valido, messaggio di errore e si riabilita il timer
	else{
		writeMessage("Position not valid");
		enable_timer(0);
	}
}

// Annullamento muro
void undoWall(){
	int dir;
	Coordinates pos;
	
	// Muro annullato
	ms.pendingWall = 0;
	
	// Cancellazione muro
	pos = ms.walls[ms.player].position[ms.walls[ms.player].used];
	dir = ms.walls[ms.player].dir[ms.walls[ms.player].used];
	drawWall(pos.x, pos.y, dir, BGCOLOR);
	
	// Ridisegno muri già presenti
	redrawWalls();
	
	// Evidenziazione celle valide per spostamento
	highliteAdj(ms.currentPos[ms.player]);
}

// Impostazione nuovo muro
void setNextWall(int h, int v){
	int i;
	Coordinates pos;
	
	// Cancellazione eventuale messaggio
	clearMessage();
	
	// Get posizione spostata del muro
	i = ms.walls[ms.player].used;
	pos = changeCoord(ms.walls[ms.player].position[i], h, v);
	
	// Se il muro si può spostare senza uscire dalla scacchiera,
	// lo si sposta, altrimenti si stampa un messaggio di errore
	if(centerWallInPlatform(pos)){
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

// Invio mossa
void sendMove(){
	
	int i;
	unsigned char dataVett[4]; // {X, Y, wallInfo, playerId}
	
	// Assegnazione dato
	for(i = 0; i < 4; i++){
		dataVett[i] = (ms.lastMove >> (i << 3)) & 0xFF;
	}
	
	// Invio messaggio
	CAN_buildMsg(1, dataVett, 4, STANDARD_FORMAT, DATA_FRAME);
	CAN_wrMsg(1, &CAN_TxMsg);
}

// Set nuova scelta
void setNextChoice(int step){
	
	// No movimento "circolare" del joystick per scegliere
	if(step == provChoice){
		return;
	}
	
	provChoice = 1 - provChoice;
	
	// Evidenziazione contorno scelta
	highliteChoice(provChoice);
}

// Conferma nuova scelta
void confirmChoice(){
	unsigned char dataVett[2];
	
	// Scelta iniziale
	if(gm.numBoards == 0){
		
		// Termine handshake iniziale, se ancora in corso
		if(gm.handshake == HANDSHAKE_ON || gm.handshake == HANDSHAKE_OFF){
			gm.handshake = HANDSHAKE_DONE;
		}
		
		// Selezione numero boards
		gm.numBoards = provChoice + 1;
		
		// Disegno menu successivo
		drawMenu(MENU_MESSAGES[provChoice][0], MENU_MESSAGES[provChoice][1],
			MENU_MESSAGES[provChoice][2], MENU_MESSAGES[provChoice][3]);
		
		// Reset scelta
		provChoice = 0;
	}
	else{
		
		// Scelta avversario
		if(gm.numBoards == 1){
			
			// Noto il tipo di entrambi i giocatori
			gm.playersType[0] = HUMAN;
			gm.playersType[1] = provChoice;
			
			// Inizializzazione modalità gioco -> Inizia l'umano (PLAYER1)
			setMode(PLAYING);
			
			// Inizializzazione interfaccia e dati dei giocatori
			initInterface();
			initPlayers();
			
			// Inizia il turno il giocatore 1
			setPlayer(PLAYER1);
			
			// Riabilitazione KEY1
			enable_button(KEY1_PIN, EINT1_IRQn);
		}
		else{	// if gm.numBoards == 2
			
			// Noto solo il tipo del proprio giocatore
			gm.playersType[gm.boardPlayer] = provChoice;
			
			// Invio messaggio di ready (si suppone che entrambi scelgano two boards)
			dataVett[0] = HANDSHAKE_PREFIX;
			dataVett[1] = HANDSHAKE_READY;
			
			CAN_buildMsg(1, dataVett, 2, STANDARD_FORMAT, DATA_FRAME);
			CAN_wrMsg(1, &CAN_TxMsg);
			
			// Inizializzazione modalità gioco: giocatore pronto a giocare
			setMode(READY);
			
			// Inizializzazione interfaccia e dati dei giocatori
			initInterface();
			initPlayers();
			
			// Attesa per avversario pronto o per mossa avversario
		}
	}
}

// Aggiornamento stato gioco sulla board corrente, data la mossa avversaria
void updateOpponentData(unsigned char playerId, unsigned char moveType, unsigned char wallDir, int y, int x){
	
	// Mossa pedina
	if(moveType == PLAYER_MOVE){
		
		// Se non fuori tempo, necessario aggiornamento
		if(wallDir != OUT_OF_TIME_MOVE){
			
			// Cancellazione evidenziazione avversario
			drawSquareArea(ms.currentPos[playerId].x, ms.currentPos[playerId].y, BGCOLOR);
			
			// Impostazione posizione avversario
			ms.currentPos[playerId] = newCoord(x, y);
			
			// Disegno pedina avversario
			drawToken(ms.currentPos[playerId].x, ms.currentPos[playerId].y, PLAYER_COLORS[playerId]);
		}
	}
	// Posizionamento muro
	else{
		
		// Impostazione
		setWall(newCoord(x, y), wallDir, playerId);
		
		// Conferma
		ms.walls[playerId].used++;
		
		// Disegno
		drawWall(x, y, wallDir, WALL_COLORS[playerId]);
	}
}

// Aggiornamento turno con messa in attesa del giocatore corrente
void setOpponentTurn(){
	
	// Reset e stop timer
	disable_timer(0);
	reset_timer(0);
	
	// Cancellazione tempo rimanente
	writeTimeRemaining(ms.timeRemaining, BGCOLOR);
	
	// Cancellazione messaggio
	clearMessage();
	
	// Cancellazione evidenziazione cella giocatore corrente
	drawSquareArea(ms.currentPos[ms.player].x, ms.currentPos[ms.player].y, BGCOLOR);
	drawToken(ms.currentPos[ms.player].x, ms.currentPos[ms.player].y, PLAYER_COLORS[ms.player]);
	
	// Cambio giocatore
	ms.player = getOtherPlayer(ms.player);
	
	// Impostazione evidenziazione cella giocatore avversario
	drawSquareArea(ms.currentPos[ms.player].x, ms.currentPos[ms.player].y, TOKEN_BGCOLOR);
	drawToken(ms.currentPos[ms.player].x, ms.currentPos[ms.player].y, PLAYER_COLORS[ms.player]);
	
}
