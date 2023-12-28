#ifndef _QUEUE_H
#define _QUEUE_H

#include "../quoridor/quoridor.h"

int isEmpty(Coordinates *queue, int dim);
void initQueue(Coordinates *queue, int *dim);
void enqueue(Coordinates *queue, Coordinates elem, int *dim);
Coordinates dequeue(Coordinates *queue, int *dim);
void clearQueue(Coordinates *queue, int *dim);

#endif
