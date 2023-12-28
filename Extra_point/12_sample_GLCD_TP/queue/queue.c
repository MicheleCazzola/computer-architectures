#include "../queue/queue.h"

static int head, tail;

int isEmpty(Coordinates *queue, int dim){
	//return dim == 0;
	return head == tail-1;
}

void initQueue(Coordinates *queue, int *dim){
	*dim = 0;
	head = -1;
	tail = 0;
}

void enqueue(Coordinates *queue, Coordinates elem, int *dim){
	//queue[(*dim)++ + head] = elem;
	queue[tail++] = elem;
}

Coordinates dequeue(Coordinates *queue, int *dim){
	int i;
	Coordinates result = queue[++head];
	/*
	for(i = 0; i < (*dim)-1; i++){
		queue[i] = queue[i+1];
		queue[i+1].x = queue[i+1].y = 0;
	}
	*/
	//(*dim)--;
	return result;
}

void clearQueue(Coordinates *queue, int *dim){
	*dim = 0;
	head = -1;
	tail = 0;
}
