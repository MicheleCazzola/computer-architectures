#include "../queue/queue.h"

static int head, tail;

int isEmpty(Coordinates *queue){
	return ((head + 1) % QUEUE_DIM) == tail;
}

void initQueue(Coordinates *queue){
	
	int i;
	for(i = 0; i < QUEUE_DIM; i++){
		queue[i].x = -1;
		queue[i].y = -1;
	}
	head = -1;
	tail = 0;
}

void enqueue(Coordinates *queue, Coordinates elem){
	/*
	if(((tail + 1) % QUEUE_DIM) == head){
		writeMessage("Queue overflow");
	}
	*/
	queue[tail % QUEUE_DIM] = elem;
	tail = (tail + 1) % QUEUE_DIM;
}

Coordinates dequeue(Coordinates *queue){
	Coordinates result;
	
	head = (head + 1) % QUEUE_DIM;
	result = queue[head % QUEUE_DIM];
	
	return result;
}
