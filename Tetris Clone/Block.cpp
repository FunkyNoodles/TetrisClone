#include "Block.h"

Block::Block(int column, int row, int colorValue) {
	this->colorValue = colorValue;
	this->column = column;
	this->row = row;

	setVertices();
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 1;
	indices[4] = 2;
	indices[5] = 3;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	setVAO();
}

void Block::setVertices() {
	// Lower left corner pos
	vertices[0] = (WIDTH - ((double)(GRID_WIDTH - column)*TILE_WIDTH)) / (WIDTH / 2) - 1.0;
	vertices[1] = (HEIGHT - ((double)(GRID_HEIGHT - row)*TILE_WIDTH)) / (HEIGHT / 2) - 1.0;
	vertices[2] = 1.0;
	// Lower right corner pos
	vertices[6] = (WIDTH - ((double)(GRID_WIDTH - column - 1)*TILE_WIDTH)) / (WIDTH / 2) - 1.0;
	vertices[7] = (HEIGHT - ((double)(GRID_HEIGHT - row)*TILE_WIDTH)) / (HEIGHT / 2) - 1.0;
	vertices[8] = 1.0;
	// Upper left corner pos
	vertices[12] = (WIDTH - ((double)(GRID_WIDTH - column)*TILE_WIDTH)) / (WIDTH / 2) - 1.0;
	vertices[13] = (HEIGHT - ((double)(GRID_HEIGHT - row - 1)*TILE_WIDTH)) / (HEIGHT / 2) - 1.0;
	vertices[14] = 1.0;
	// Upper right corner pos
	vertices[18] = (WIDTH - ((double)(GRID_WIDTH - column - 1)*TILE_WIDTH)) / (WIDTH / 2) - 1.0;
	vertices[19] = (HEIGHT - ((double)(GRID_HEIGHT - row - 1)*TILE_WIDTH)) / (HEIGHT / 2) - 1.0;
	vertices[20] = 1.0;

	// Color is the same so a loop will do
	for (int i = 0; i < 4; i++)
	{
		switch (colorValue)
		{
		case 1:
			// Cyan
			vertices[6 * i + 3] = 0.0;
			vertices[6 * i + 4] = 1.0;
			vertices[6 * i + 5] = 1.0;
			break;
		case 2:
			// Blue
			vertices[6 * i + 3] = 0.0;
			vertices[6 * i + 4] = 0.0;
			vertices[6 * i + 5] = 1.0;
			break;
		case 3:
			// Orange
			vertices[6 * i + 3] = 1.0;
			vertices[6 * i + 4] = 0.6470588235;
			vertices[6 * i + 5] = 0.0;
			break;
		case 4:
			// Yellow
			vertices[6 * i + 3] = 1.0;
			vertices[6 * i + 4] = 1.0;
			vertices[6 * i + 5] = 0.0;
			break;
		case 5:
			// Green
			vertices[6 * i + 3] = 0.0;
			vertices[6 * i + 4] = 1.0;
			vertices[6 * i + 5] = 0.0;
			break;
		case 6:
			// Purple
			vertices[6 * i + 3] = 0.6666666667;
			vertices[6 * i + 4] = 0.0;
			vertices[6 * i + 5] = 1.0;
			break;
		case 7:
			// Orange
			vertices[6 * i + 3] = 1.0;
			vertices[6 * i + 4] = 0.0;
			vertices[6 * i + 5] = 0.0;
			break;
		case 8:
			// Projection block - grey
			vertices[6 * i + 3] = 0.5;
			vertices[6 * i + 4] = 0.5;
			vertices[6 * i + 5] = 0.5;
		default:
			break;
		}
	}
}

void Block::setVAO() {

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(GLdouble), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(GLdouble), (GLvoid*)(3 * sizeof(GLdouble)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

int Block::getColorValue() {
	return colorValue;
}

int Block::getColumn() {
	return column;
}

int Block::getRow() {
	return row;
}

void Block::setColorValue(int colorValue) {
	this->colorValue = colorValue;
}

void Block::setColumn(int column) {
	this->column = column;
}

void Block::setRow(int row) {
	this->row = row;
}

Block::~Block() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}