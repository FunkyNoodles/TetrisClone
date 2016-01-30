#pragma once
// GLEW
#define GLEW_STATIC
#include <glew.h>

// GLFW
#include <glfw3.h>

// Other includes 
#include "constants.h"
#include "Shader.h"
#include "Coord.h"
#include "resource.h"
#include "Block.h"

class Controller {
public:

	
	// In coords or block arrays, the first element holds the center piece

	// Coords of the controller block
	Coord coords[4];

	// Holds what coords will be after rotation
	Coord rotationBuffer[4];

	// Controller blocks to render
	Block* blocks[4];

	// Coords of where the projection is
	Coord projectionBuffer[4];

	// Projection blocks to render
	Block* projectionBlocks[4];

	Controller() {};

	Controller(int c0, int r0, int c1, int r1, int c2, int r2, int c3, int r3, int colorValue);

	void setController(int c0, int r0, int c1, int r1, int c2, int r2, int c3, int r3, int colorValue);

	// Move a set of coords down
	void moveDown(Coord(&coords)[4]);

	// Move a set of coords up
	void moveUp(Coord(&coords)[4]);

	// Move a set of coords left
	void moveLeft(Coord(&coords)[4]);

	// Move a set of coords right
	void moveRight(Coord(&coords)[4]);

	// Update blocks to render
	void updateBlocks();

	// Check if a set of coords can move down
	bool canMoveDown(Coord(&coords)[4]);

	// Check if a set of coords can move up
	bool canMoveUp(Coord(&coords)[4]);

	// Check if a set of coords can move left
	bool canMoveLeft(Coord(&coords)[4]);

	// Check if a set of coords can move right
	bool canMoveRight(Coord(&coords)[4]);

	// Check if a set of coords is right above anything
	bool isTouchingDown(Coord(&coords)[4]);

	// Check if a set of coords is right below anything
	bool isTouchingUp(Coord(&coords)[4]);

	// Check if a set of coords is to the right of anything
	bool isTouchingLeft(Coord(&coords)[4]);

	// Check if a set of coords is to the left anything
	bool isTouchingRight(Coord(&coords)[4]);

	// Rotate the piece 90 degrees clockwise
	void rotate();

	// Check if the piece can rotate
	bool canRotate();

	// Set rotation buffer, and returns true if new rotation buffer is out of bound
	// Use rotation matrix to set a rotation buffer
	// x -> -y
	// y -> x
	// a special case when the angle is 90 degrees about z-axis
	bool updateRotationBuffer();

	// Check if a coord is in bound of the well
	bool isInBound(Coord coord);

	// Check if a coord is in bound of the well three sides
	bool isInBoundThreeSides(Coord coord);

	// Check if a set of coords is out of bound of the well
	bool isOutOfBound(Coord(&coords)[4]);

	// Check if a coord is out of bound of the well
	bool isOutOfBound(Coord &coord);

	// Check if a set of coords is out of the borders of the well (excluding the top side)
	bool isOutOfBoundThreeSides(Coord(&coords)[4]);

	// Check if a coord is out of the borders of the well (excluding the top side)
	bool isOutOfBoundThreeSides(Coord &coord);

	// Check if a set of coords overlaps existing blocks
	bool isOverlapping(Coord(&coords)[4]);

	// Check if a coord overlaps existing blocks
	bool isOverlapping(Coord &coord);

	// Update where project block is
	void updateBlockProjection();

	int Controller::getColorValue();

	void Controller::setColorValue(int colorValue);

	Controller::~Controller();

	// Move the coords to projected location
	int Controller::appendDown();

	// Print the coords
	void Controller::printCoords();
private:
	// Color value implies the shape
	int colorValue;
};