#ifndef _QUEUE_H
#define _QUEUE_H

#include "../coordinates/coordinates.h"

// Dimensione coda
#define QUEUE_DIM 20

int isEmpty(Coordinates *queue);
void initQueue(Coordinates *queue);
void enqueue(Coordinates *queue, Coordinates elem);
Coordinates dequeue(Coordinates *queue);

#endif
