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

class Block {
public:
	GLuint VAO, VBO, EBO;
	GLuint indices[6];
	Block(int column, int row, int colorValue);

	void setVertices();

	void setVAO();

	int getColorValue();

	int getColumn();

	int getRow();

	void setColorValue(int colorValue);

	void setColumn(int column);

	void setRow(int row);

	~Block();
private:
	GLdouble vertices[24]; // 3 pos 3 color

	int colorValue;
	int column;
	int row;
};