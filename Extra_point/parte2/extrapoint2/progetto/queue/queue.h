#ifndef _QUEUE_H
#define _QUEUE_H

#include "../coordinates/coordinates.h"

// Dimensione coda: contiene al massimo metà degli elementi totali
#define QUEUE_DIM (NUM_SQUARES * NUM_SQUARES/2 + 1)

int isEmpty(Coordinates *queue);
void initQueue(Coordinates *queue);
void enqueue(Coordinates *queue, Coordinates elem);
Coordinates dequeue(Coordinates *queue);

#endif
