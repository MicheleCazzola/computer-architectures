#include "../queue/queue.h"

int is_empty(Coordinates *queue, int dim){
	return dim == 0;
}

void enqueue(Coordinates *queue, Coordinates elem, int *dim){
	queue[(*dim)++] = elem;
}

Coordinates dequeue(Coordinates *queue, int *dim){
	int i;
	Coordinates result = queue[0];
	for(i = 0; i < (*dim)-1; i++){
		queue[i] = queue[i+1];
	}
	(*dim)--;
	return result;
}

void clear_queue(Coordinates *queue, int *dim){
	*dim = 0;
}