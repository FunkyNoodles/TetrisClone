#pragma once
// Screen
const int WIDTH = 800;
const int HEIGHT = 600;
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 20;
const int TILE_WIDTH = HEIGHT / GRID_HEIGHT; // Tile width of a tile is demermined by window pixel height

extern int grid[GRID_WIDTH][GRID_HEIGHT];

/*
* Block information
* -1 - to be replaced by above row
* 0 - nothing
* 1 - occupied by I
* 2 - occupied by J
* 3 - occupied by L
* 4 - occupied by O
* 5 - occupied by S
* 6 - occupied by T
* 7 - occupied by Z
*/
const int BLOCK_TO_REPLACE = -1;
const int BLOCK_EMPTY = 0;
const int BLOCK_I = 1;
const int BLOCK_J = 2;
const int BLOCK_L = 3;
const int BLOCK_O = 4;
const int BLOCK_S = 5;
const int BLOCK_T = 6;
const int BLOCK_Z = 7;