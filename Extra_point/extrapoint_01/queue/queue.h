#ifndef _QUEUE_H
#define _QUEUE_H

#include "../coordinates/coordinates.h"

// Dimensione coda: contiene al massimo 25 elementi (metà del totale)
#define QUEUE_DIM 26

int isEmpty(Coordinates *queue);
void initQueue(Coordinates *queue);
void enqueue(Coordinates *queue, Coordinates elem);
Coordinates dequeue(Coordinates *queue);

#endif
