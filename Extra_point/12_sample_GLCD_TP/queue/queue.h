#ifndef _QUEUE_H
#define _QUEUE_H

#include "../quoridor/quoridor.h"

int is_empty(Coordinates *queue, int dim);
void enqueue(Coordinates *queue, Coordinates elem, int *dim);
Coordinates dequeue(Coordinates *queue, int *dim);
void clear_queue(Coordinates *queue, int *dim);

#endif