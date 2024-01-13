#include "../queue/queue.h"
#include "../graphics/interface.h"

#define DEFAULT_COORD -1

// Testa e coda della coda di elementi
static int head, tail;

// Check se coda vuota
int isEmpty(Coordinates *queue){
	return ((head + 1) % QUEUE_DIM) == tail;
}

// Inizializzazione coda
void initQueue(Coordinates *queue){
	
	int i;
	for(i = 0; i < QUEUE_DIM; i++){
		queue[i] = newCoord(DEFAULT_COORD, DEFAULT_COORD);
	}
	
	head = -1;
	tail = 0;
}

// Inserimento nella coda (in coda)
void enqueue(Coordinates *queue, Coordinates elem){
	queue[tail % QUEUE_DIM] = elem;
	tail = (tail + 1) % QUEUE_DIM;
}

// Estrazione dalla coda (in testa)
Coordinates dequeue(Coordinates *queue){
	Coordinates result;
	
	head = (head + 1) % QUEUE_DIM;
	result = queue[head % QUEUE_DIM];
	
	return result;
}
