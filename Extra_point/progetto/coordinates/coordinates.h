#ifndef _COORDINATES_H_
#define _COORDINATES_H_

// Coordinate della posizione
// Origine del sistema di riferimento nella cella in basso a sinistra
typedef struct {
	int x;
	int y;
} Coordinates;

Coordinates newCoord(int x, int y);
Coordinates changeCoord(Coordinates currentPos, int h, int v);
int equalCoordX(Coordinates a, Coordinates b);
int equalCoordY(Coordinates a, Coordinates b);
int equalCoord(Coordinates a, Coordinates b);
int coordCmpX(Coordinates a, Coordinates b);
int coordCmpY(Coordinates a, Coordinates b);
int coordDiffX(Coordinates a, Coordinates b);
int coordDiffY(Coordinates a, Coordinates b);
int coordDistX(Coordinates a, Coordinates b);
int coordDistY(Coordinates a, Coordinates b);
#endif
