#include "../queue/queue.h"

static int head, tail;

int isEmpty(Coordinates *queue){
	return head == tail-1;
}

void initQueue(Coordinates *queue){
	head = -1;
	tail = 0;
}

void enqueue(Coordinates *queue, Coordinates elem){
	queue[tail++] = elem;
}

Coordinates dequeue(Coordinates *queue){
	Coordinates result = queue[++head];
	return result;
}

void clearQueue(Coordinates *queue){
	head = -1;
	tail = 0;
}
