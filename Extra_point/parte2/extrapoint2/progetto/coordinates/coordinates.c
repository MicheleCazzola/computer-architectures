#include "coordinates.h"

static int abs(int x){
	return x > 0 ? x : -x;
}

// Creazione nuova coordinata
Coordinates newCoord(int x, int y){
	Coordinates c;
	
	c.x = x;
	c.y = y;
	
	return c;
}

// Spostamento posizione, date quantità orizzontale e verticale
Coordinates changeCoord(Coordinates currentPos, int h, int v){
	Coordinates newPos;
	
	newPos.x = currentPos.x + h;
	newPos.y = currentPos.y + v;
	
	return newPos;
}

// Check uguale X
int equalCoordX(Coordinates a, Coordinates b){
	return a.x == b.x;
}

// Check uguale Y
int equalCoordY(Coordinates a, Coordinates b){
	return a.y == b.y;
}

// Check posizione uguale
int equalCoord(Coordinates a, Coordinates b){
	return equalCoordX(a, b) && equalCoordY(a, b);
}

// Confronto su X
int coordCmpX(Coordinates a, Coordinates b){
	return a.x - b.x;
}

// Confronto su Y
int coordCmpY(Coordinates a, Coordinates b){
	return a.y - b.y;
}

// Differenza su X
int coordDiffX(Coordinates a, Coordinates b){
	return a.x - b.x;
}

// Differenza su Y
int coordDiffY(Coordinates a, Coordinates b){
	return a.y - b.y;
}

// Differenza su (x,y)
Coordinates coordDiff(Coordinates a, Coordinates b){
	Coordinates result;
	result.x = coordDiffX(a, b);
	result.y = coordDiffY(a, b);
	
	return result;
}

// Distanza su X
int coordDistX(Coordinates a, Coordinates b){
	return abs(coordDiffX(a, b));
}

// Distanza su Y
int coordDistY(Coordinates a, Coordinates b){
	return abs(coordDiffY(a, b));
}
