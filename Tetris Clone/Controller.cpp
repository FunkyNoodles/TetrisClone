#include "Controller.h"

Controller::Controller(int c0, int r0, int c1, int r1, int c2, int r2, int c3, int r3, int colorValue) {
	coords[0] = Coord(c0, r0);
	coords[1] = Coord(c1, r1);
	coords[2] = Coord(c2, r2);
	coords[3] = Coord(c3, r3);
	rotationBuffer[0] = Coord(c0, r0);
	rotationBuffer[1] = Coord(c1, r1);
	rotationBuffer[2] = Coord(c2, r2);
	rotationBuffer[3] = Coord(c3, r3);
	projectionBuffer[0] = Coord(c0, r0);
	projectionBuffer[1] = Coord(c1, r1);
	projectionBuffer[2] = Coord(c2, r2);
	projectionBuffer[3] = Coord(c3, r3);
	this->colorValue = colorValue;
}

void Controller::setController(int c0, int r0, int c1, int r1, int c2, int r2, int c3, int r3, int colorValue) {
	coords[0].setColumn(c0);
	coords[0].setRow(r0);
	coords[1].setColumn(c1);
	coords[1].setRow(r1);
	coords[2].setColumn(c2);
	coords[2].setRow(r2);
	coords[3].setColumn(c3);
	coords[3].setRow(r3);

	projectionBuffer[0].setColumn(c0);
	projectionBuffer[0].setRow(r0);
	projectionBuffer[1].setColumn(c1);
	projectionBuffer[1].setRow(r1);
	projectionBuffer[2].setColumn(c2);
	projectionBuffer[2].setRow(r2);
	projectionBuffer[3].setColumn(c3);
	projectionBuffer[3].setRow(r3);
	this->colorValue = colorValue;

	// Set blocks to render
	for (int i = 0; i < 4; i++)
	{
		if (blocks[i] != nullptr)
		{
			delete blocks[i];
			blocks[i] = nullptr;
		}
		blocks[i] = new Block(coords[i].getColumn(), coords[i].getRow(), colorValue);
		if (projectionBlocks[i] != nullptr)
		{
			delete projectionBlocks[i];
			projectionBlocks[i] = nullptr;
		}
		projectionBlocks[i] = new Block(coords[i].getColumn(), coords[i].getRow(), 8);
	}
}

void Controller::moveDown(Coord(&coords)[4]) {
	if (canMoveDown(coords))
	{
		for (int i = 0; i < 4; i++)
		{
			coords[i].setRow(coords[i].getRow() - 1);
		}
	}
	updateBlocks();
}

void Controller::moveUp(Coord(&coords)[4]) {
	if (canMoveUp(coords))
	{
		for (int i = 0; i < 4; i++)
		{
			coords[i].setRow(coords[i].getRow() + 1);
		}
	}
	updateBlocks();
}

void Controller::moveLeft(Coord(&coords)[4]) {
	if (canMoveLeft(coords))
	{
		for (int i = 0; i < 4; i++)
		{
			coords[i].setColumn(coords[i].getColumn() - 1);
		}
	}
	updateBlocks();
}

void Controller::moveRight(Coord(&coords)[4]) {
	if (canMoveRight(coords))
	{
		for (int i = 0; i < 4; i++)
		{
			coords[i].setColumn(coords[i].getColumn() + 1);
		}
	}
	updateBlocks();
}

void Controller::updateBlocks() {
	for (int i = 0; i < 4; i++)
	{
		blocks[i]->setColorValue(colorValue);
		blocks[i]->setColumn(coords[i].getColumn());
		blocks[i]->setRow(coords[i].getRow());
		blocks[i]->setVertices();
		blocks[i]->setVAO();
	}
}

bool Controller::canMoveDown(Coord(&coords)[4]) {
	for (int i = 0; i < 4; i++)
	{
		// Check if in range
		if (isInBoundThreeSides(coords[i]))
		{
			if (isInBound(coords[i]) || coords[i].getRow() == GRID_HEIGHT)
			{
				if (coords[i].getRow() == 0 || grid[coords[i].getColumn()][coords[i].getRow() - 1] != 0) {
					return false;
				}
			}
		}
	}
	return true;
}

bool Controller::canMoveUp(Coord(&coords)[4]) {
	for (int i = 0; i < 4; i++)
	{
		// Cehck if in range
		if (isInBoundThreeSides(coords[i]))
		{
			if (isInBound(coords[i]))
			{
				if (grid[coords[i].getColumn()][coords[i].getRow() + 1] != 0) {
					return false;
				}
			}
		}
	}
	return true;
}

bool Controller::canMoveLeft(Coord(&coords)[4]) {
	for (int i = 0; i < 4; i++)
	{
		if (isInBoundThreeSides(coords[i]))
		{
			if (coords[i].getColumn() == 0)
			{
				return false;
			}
			if (isInBound(coords[i]))
			{
				if (grid[coords[i].getColumn() - 1][coords[i].getRow()] != 0) {
					return false;
				}
			}
		}
	}
	return true;
}

bool Controller::canMoveRight(Coord(&coords)[4]) {
	for (int i = 0; i < 4; i++)
	{
		if (isInBoundThreeSides(coords[i]))
		{
			if (coords[i].getColumn() == GRID_WIDTH - 1)
			{
				return false;
			}
			if (isInBound(coords[i]))
			{
				if (grid[coords[i].getColumn() + 1][coords[i].getRow()] != 0) {
					return false;
				}
			}
		}
	}
	return true;
}

bool Controller::isTouchingDown(Coord(&coords)[4]) {
	bool value = false;
	if (!canMoveDown(coords) && !isOverlapping(coords))
	{
		for (int i = 0; i < 4; i++)
		{
			if (isInBoundThreeSides(coords[i]))
			{
				if (coords[i].getRow() == 0)
				{
					value = value || true;
				}
				else if (grid[coords[i].getColumn()][coords[i].getRow() - 1] != 0)
				{
					value = value || true;
				}
				else
				{
					value = value || false;
				}
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return value;
}

bool Controller::isTouchingUp(Coord(&coords)[4]) {
	bool value = false;;
	if (!canMoveUp(coords) && !isOverlapping(coords))
	{
		for (int i = 0; i < 4; i++)
		{
			if (isInBound(coords[i]))
			{
				if (coords[i].getRow() == GRID_HEIGHT - 1)
				{
					value = value || true;
				}
				else if (grid[coords[i].getColumn()][coords[i].getRow() + 1] != 0)
				{
					value = value || true;
				}
				else
				{
					value = value || false;
				}
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return value;
}
bool Controller::isTouchingLeft(Coord(&coords)[4]) {
	bool value = false;
	if (!canMoveLeft(coords) && !isOverlapping(coords))
	{
		for (int i = 0; i < 4; i++)
		{
			if (isInBound(coords[i]))
			{
				if (coords[i].getColumn() == 0 && !isOverlapping(coords[i]))
				{
					value = value || true;
				}
				else if (grid[coords[i].getColumn() - 1][coords[i].getRow()] != 0)
				{
					value = value || true;
				}
				else
				{
					value = value || false;
				}
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return value;
}

bool Controller::isTouchingRight(Coord(&coords)[4]) {
	bool value = false;
	if (!canMoveRight(coords) && !isOverlapping(coords))
	{
		for (int i = 0; i < 4; i++)
		{
			if (isInBound(coords[i]))
			{
				if (coords[i].getColumn() == GRID_WIDTH - 1)
				{
					value = value || true;
				}
				else if (grid[coords[i].getColumn() + 1][coords[i].getRow()] != 0)
				{
					value = value || true;
				}
				else
				{
					value = value || false;
				}
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return value;
}

void Controller::rotate() {
	// Disable rotation for O
	if (canRotate() && colorValue != 4)
	{
		// If allows rotation, copy from rotationBuffer
		for (int i = 0; i < 4; i++)
		{
			coords[i].setColumn(rotationBuffer[i].getColumn());
			coords[i].setRow(rotationBuffer[i].getRow());
		}
	}
	updateBlocks();
}

bool Controller::canRotate() {
	bool isFirst = true;
	updateRotationBuffer();
	if (isOutOfBoundThreeSides(rotationBuffer) || isOverlapping(rotationBuffer))
	{
		isFirst = true;
		while (!canMoveDown(rotationBuffer) && canMoveUp(rotationBuffer))
		{
			if (isTouchingDown(rotationBuffer) && !isFirst)
			{
				break;
			}
			else
			{
				moveUp(rotationBuffer);
			}
			isFirst = false;
		}
		/*isFirst = true;
		while (!canMoveUp(rotationBuffer) && canMoveDown(rotationBuffer))
		{
		if (isTouchingUp(rotationBuffer) && !isFirst)
		{
		break;
		}
		else
		{
		moveDown(rotationBuffer);
		}
		isFirst = false;
		}*/
		isFirst = true;
		while (!canMoveLeft(rotationBuffer) && canMoveRight(rotationBuffer))
		{
			if (isTouchingLeft(rotationBuffer) && !isFirst)
			{
				break;
			}
			else
			{
				moveRight(rotationBuffer);
			}
			isFirst = false;
		}
		isFirst = true;
		while (!canMoveRight(rotationBuffer) && canMoveLeft(rotationBuffer))
		{
			if (isTouchingRight(rotationBuffer) && !isFirst)
			{
				break;
			}
			else
			{
				moveLeft(rotationBuffer);
			}
			isFirst = false;
		}
		// Check again after translations
		if (isOutOfBoundThreeSides(rotationBuffer))
		{
			// Still out of bound
			return false;
		}
		if (isOverlapping(rotationBuffer))
		{
			return false;
		}
	}
	return true;
}

bool Controller::updateRotationBuffer() {
	// x and y with respect to center piece
	int deltaX, deltaY;
	bool isOutOfBound = false;
	for (int i = 0; i < 4; i++)
	{
		deltaX = coords[i].getColumn() - coords[0].getColumn();
		deltaY = coords[i].getRow() - coords[0].getRow();
		rotationBuffer[i].setColumn(coords[0].getColumn() - deltaY);
		rotationBuffer[i].setRow(coords[0].getRow() + deltaX);

		// Check if out of bound
		if (rotationBuffer[i].getColumn() >= GRID_WIDTH || rotationBuffer[i].getColumn() < 0 || rotationBuffer[i].getRow() < 0)
		{
			isOutOfBound = true;
		}
	}
	return isOutOfBound;
}

bool Controller::isInBound(Coord coord) {
	if (coord.getColumn() < 0 || coord.getColumn() >= GRID_WIDTH || coord.getRow() < 0 || coord.getRow() >= GRID_HEIGHT)
	{
		return false;
	}
	return true;
}

bool Controller::isInBoundThreeSides(Coord coord) {
	if (coord.getColumn() < 0 || coord.getColumn() >= GRID_WIDTH || coord.getRow() < 0)
	{
		return false;
	}
	return true;
}

bool Controller::isOutOfBound(Coord(&coords)[4]) {
	for (int i = 0; i < 4; i++)
	{
		if (coords[i].getColumn() < 0 || coords[i].getColumn() >= GRID_WIDTH || coords[i].getRow() < 0 || coords[i].getRow() >= GRID_HEIGHT)
		{
			return true;
		}
	}
	return false;
}

bool Controller::isOutOfBound(Coord &coord) {
	if (coord.getColumn() < 0 || coord.getColumn() >= GRID_WIDTH || coord.getRow() <0 || coord.getRow() >= GRID_HEIGHT)
	{
		return true;
	}
	return false;
}

bool Controller::isOutOfBoundThreeSides(Coord(&coords)[4]) {
	for (int i = 0; i < 4; i++)
	{
		if (coords[i].getColumn() < 0 || coords[i].getColumn() >= GRID_WIDTH || coords[i].getRow() < 0)
		{
			return true;
		}
	}
	return false;
}

bool Controller::isOutOfBoundThreeSides(Coord &coord) {
	if (coord.getColumn() < 0 || coord.getColumn() >= GRID_WIDTH || coord.getRow() < 0)
	{
		return true;
	}
	return false;
}

bool Controller::isOverlapping(Coord(&coords)[4]) {
	for (int i = 0; i < 4; i++)
	{
		if (!isOutOfBound(coords[i]))
		{
			if (grid[coords[i].getColumn()][coords[i].getRow()] != 0)
			{
				return true;
			}
		}
	}
	return false;
}

bool Controller::isOverlapping(Coord &coord) {
	if (!isOutOfBound(coord))
	{
		if (grid[coord.getColumn()][coord.getRow()] != 0)
		{
			return true;
		}
	}
	return false;
}

void Controller::updateBlockProjection() {
	// Set projection block position to controller
	for (int i = 0; i < 4; i++)
	{
		projectionBuffer[i].setColumn(coords[i].getColumn());
		projectionBuffer[i].setRow(coords[i].getRow());
	}

	// Find where the projection is
	while (canMoveDown(projectionBuffer))
	{
		moveDown(projectionBuffer);
	}

	// Update projection blocks render
	for (int i = 0; i < 4; i++)
	{
		projectionBlocks[i]->setColumn(projectionBuffer[i].getColumn());
		projectionBlocks[i]->setRow(projectionBuffer[i].getRow());
		projectionBlocks[i]->setVertices();
		projectionBlocks[i]->setVAO();
	}
}

int Controller::getColorValue() {
	return colorValue;
}

void Controller::setColorValue(int colorValue) {
	this->colorValue = colorValue;
}

Controller::~Controller() {
	for (int i = 0; i < 4; i++)
	{
		if (blocks[i] != nullptr)
		{
			delete blocks[i];
			blocks[i] = nullptr;
		}
		if (projectionBlocks[i] != nullptr)
		{
			delete projectionBlocks[i];
			projectionBlocks[i] = nullptr;
		}
	}
}

int Controller::appendDown() {
	int skippedRows = 0;
	skippedRows = coords[0].getRow() - projectionBuffer[0].getRow();
	for (int i = 0; i < 4; i++)
	{
		coords[i] = projectionBuffer[i];
	}
	updateBlocks();
	return skippedRows;
}

void Controller::printCoords() {
	std::cout << "Controller Coords: " << std::endl;
	for (int i = 0; i < 4; i++)
	{
		std::cout << i << ": x=" << coords[i].getColumn() << ", y=" << coords[i].getRow() << std::endl;
	}
}