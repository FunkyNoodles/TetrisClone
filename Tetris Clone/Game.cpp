// System
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
// GLEW
#define GLEW_STATIC
#include <glew.h>

// GLFW
#include <glfw3.h>

// Image loader
#include <SOIL.h>

// GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// Other includes 
#include "Shader.h"
#include "Coord.h"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void clearGrid();
void renderBlocks();
void updateGridBlocks();
void updateGrid();
bool appendToGrid();
bool checkLineFull(int row);
void clearRow(int row);
void spawnBlock(int colorValue, int rotation);
int generateNextBlockColor();
int generateNextBlockRotation();

// Windows
const int WIDTH = 800, HEIGHT = 600;

// GL Variables
GLFWmonitor* monitor;
GLFWvidmode* desktopMode;

// Game Variables
bool fullscreen = false;
double dropSpeed = 0.7; // default drop speed
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 20;
const int TILE_WIDTH = HEIGHT / GRID_HEIGHT; // Tile width of a tile is demermined by window pixel height
/*
 * 0 - nothing
 * 1 - occupied by I
 * 2 - occupied by J
 * 3 - occupied by L
 * 4 - occupied by O
 * 5 - occupied by S
 * 6 - occupied by T
 * 7 - occupied by Z
 */
static int grid[GRID_WIDTH][GRID_HEIGHT];

class Block{
public:
	GLuint VAO, VBO, EBO;
	Shader blockShader = Shader("backColumnVertexShader.vert", "backColumnFragmentShader.frag");
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

	void Block::render() {
		
		blockShader.Use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
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
private:
	GLdouble vertices[24]; // 3 pos 3 color
	GLuint indices[6];
	int colorValue;
	int column;
	int row;
};

static Block* gridBlock[GRID_WIDTH][GRID_HEIGHT];

class Controller {
public:
	// The first element holds the center piece
	Coord coords[4];
	Block* blocks[4];

	Controller::Controller() {};

	Controller::Controller(int c0, int r0, int c1, int r1, int c2, int r2, int c3, int r3, int colorValue) {
		coords[0] = Coord(c0, r0);
		coords[1] = Coord(c1, r1);
		coords[2] = Coord(c2, r2);
		coords[3] = Coord(c3, r3);
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
		this->colorValue = colorValue;
		// Set blocks to render
		for (int i = 0; i < 4; i++)
		{
			if (blocks[i] != nullptr)
			{
				delete blocks[i];
			}
			blocks[i] = new Block(coords[i].getColumn(), coords[i].getRow(), colorValue);
		}
	}

	void Controller::moveDown() {
		if (!isSettled())
		{
			for (int i = 0; i < 4; i++)
			{
				coords[i].setRow(coords[i].getRow() - 1);
			}
		}
		updateBlocks();
	}

	void Controller::moveLeft() {
		if (canMoveLeft())
		{
			for (int i = 0; i < 4; i++)
			{
				coords[i].setColumn(coords[i].getColumn() - 1);
			}
		}
		updateBlocks();
	}

	void Controller::moveRight() {
		if (canMoveRight())
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

	bool Controller::isSettled() {
		for (int i = 0; i < 4; i++)
		{
			if (coords[i].getRow() == 0 || grid[coords[i].getColumn()][coords[i].getRow() - 1] != 0) {
				return true;
			}
		}
		return false;
	}

	bool Controller::canMoveLeft() {
		for (int i = 0; i < 4; i++)
		{
			if (coords[i].getColumn() == 0 || grid[coords[i].getColumn() - 1][coords[i].getRow()] != 0) {
				return false;
			}
		}
		return true;
	}

	bool Controller::canMoveRight() {
		for (int i = 0; i < 4; i++)
		{
			if (coords[i].getColumn() == GRID_WIDTH - 1 || grid[coords[i].getColumn() + 1][coords[i].getRow()] != 0) {
				return false;
			}
		}
		return true;
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
			}
		}
	}
private:
	// Color value implies the shape
	int colorValue;
};

static Controller controller = Controller();

// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow((GLuint)WIDTH, (GLuint)HEIGHT, "Teris Clone", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// Get monitor
	monitor = glfwGetPrimaryMonitor();

	// Build and compile shader program
	Shader backColumnsShader("backColumnVertexShader.vert", "backColumnFragmentShader.frag");

	// Basic tile information
	int pixelX = 0; // number of pixels from left border
	int pixelY = 0; // number of pixels from bottom border
	// Coords, center is 0,0, corners are +/-1
	double coordX = 0;
	double coordY = 0;

	// Set up vertices to draw background vertical columns for tiles to fall
	GLdouble backColumnVertices[GRID_WIDTH * 4 * 6]; // Format: # of columns times four sets of three position and three color
	// Set up indices to draw back as rectangles
	GLuint backColumnsIndices[GRID_WIDTH * 3 * 2]; //012,123,456,567, etc...

	//Cycle thru columns to fill backColumnVertices
	for (int i = 0; i < GRID_WIDTH; i++)
	{
		//Cycle thru vertices
		for (int j = 0; j < 4; j++)
		{
			// Get pixel x and y, 0 is bottom left, 1 is bottom right, 2 is top left, 3 is top right
			switch (j)
			{
			case 0:
				pixelX = WIDTH - TILE_WIDTH*(GRID_WIDTH - i);
				pixelY = 0;
				break;
			case 1:
				pixelX = WIDTH - TILE_WIDTH*(GRID_WIDTH - 1 - i);
				pixelY = 0;
				break;
			case 2:
				pixelX = WIDTH - TILE_WIDTH*(GRID_WIDTH - i);
				pixelY = HEIGHT;
				break;
			case 3:
				pixelX = WIDTH - TILE_WIDTH*(GRID_WIDTH - 1 - i);
				pixelY = HEIGHT;
				break;
			default:
				break;
			}
			coordX = (double)(pixelX - WIDTH / 2) / (WIDTH / 2);
			coordY = (double)(pixelY - HEIGHT / 2) / (HEIGHT / 2);
			backColumnVertices[6 * (4 * i + j)] = coordX; // posX
			backColumnVertices[6 * (4 * i + j) + 1] = coordY; // posY
			backColumnVertices[6 * (4 * i + j) + 2] = 0.0; // posZ 2D game, so 0
			// Alternate colors
			if (i % 2 == 0)
			{
				backColumnVertices[6 * (4 * i + j) + 3] = 1.0; // color R
				backColumnVertices[6 * (4 * i + j) + 4] = 1.0; // color G
				backColumnVertices[6 * (4 * i + j) + 5] = 1.0; // color B
			}
			else
			{
				backColumnVertices[6 * (4 * i + j) + 3] = 0.7; // color R
				backColumnVertices[6 * (4 * i + j) + 4] = 0.7; // color G
				backColumnVertices[6 * (4 * i + j) + 5] = 0.7; // color B
			}
			
		}
		// Fill backColumnIndices
		backColumnsIndices[6 * i] = 4 * i;
		backColumnsIndices[6 * i + 1] = 4 * i + 1;
		backColumnsIndices[6 * i + 2] = 4 * i + 2;
		backColumnsIndices[6 * i + 3] = 4 * i + 1;
		backColumnsIndices[6 * i + 4] = 4 * i + 2;
		backColumnsIndices[6 * i + 5] = 4 * i + 3;
	}
	
	GLuint backVBO, backVAO, backEBO;
	glGenVertexArrays(1, &backVAO);
	glGenBuffers(1, &backVBO);
	glGenBuffers(1, &backEBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(backVAO);

	glBindBuffer(GL_ARRAY_BUFFER, backVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(backColumnVertices), backColumnVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backColumnsIndices), backColumnsIndices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(GLdouble), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(GLdouble), (GLvoid*)(3 * sizeof(GLdouble)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO

	GLuint transformLoc = glGetUniformLocation(backColumnsShader.Program, "transform");

	clearGrid();

	spawnBlock(7, 2);

	double startTime = glfwGetTime(); // time when game starts
	double previousTime = startTime; // Time when previous update
	double currentTime;
	
	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		
		


		// Render
		// Clear the colorbuffer
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Activate shader
		backColumnsShader.Use();

		// Create transformation
		//glm::mat4 trans;
		//trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
		//trans = glm::rotate(trans, (GLfloat)glfwGetTime()*5.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		// Draw back columns
		glBindVertexArray(backVAO);
		glDrawElements(GL_TRIANGLES, sizeof(backColumnsIndices)/sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		

		currentTime = glfwGetTime() - startTime;
		if (currentTime - previousTime > dropSpeed)
		{
			// Update controller
			if (controller.isSettled())
			{
				if (!appendToGrid())
				{
					//clearGrid();
				}
				//std::cout << appendToGrid() << std::endl;
				spawnBlock(generateNextBlockColor(), generateNextBlockRotation());
			}
			else {
				controller.moveDown();

			}
			previousTime += dropSpeed;
			updateGridBlocks();
			
		}
		renderBlocks();
		//controller.moveDown();

		//std::cout << controller.coords[0].getColumn() << "  " <<  controller.coords[0].getRow() << std::endl;
		
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &backVAO);
	glDeleteBuffers(1, &backVBO);
	clearGrid();
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	/*if (key == GLFW_KEY_F11 && action == GLFW_RELEASE)
	{
		//glfwGetVideoMode(&monitor);

		fullscreen = !fullscreen;
		const GLFWvidmode* desktopMode = glfwGetVideoMode(monitor);
		//GLFWwindow* newWindow = glfwCreateWindow(desktopMode->width, desktopMode->height, "Damn", monitor, nullptr);
		//glfwDestroyWindow(window);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		
		//std::cout << desktopMode->height << std::endl;
		window = glfwCreateWindow(desktopMode->width, desktopMode->height, "Damn", monitor, nullptr);
		glfwMakeContextCurrent(window);
	}*/
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		// Test key
		for (int i = 0; i < 7; i++)
		{
			grid[i][i] = i + 1;
			grid[i + 1][i] = i + 1;
		}
		//spawnBlock(2, 1);
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		controller.moveDown();
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		controller.moveLeft();
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		controller.moveRight();
	}
}

void clearGrid() {
	for (int i = 0; i < GRID_WIDTH; i++)
	{
		for (int j = 0; j < GRID_HEIGHT; j++)
		{
			grid[i][j] = 0;
			if (gridBlock[i][j] != nullptr)
			{
				delete gridBlock[i][j];
			}
		}
	}
}

void renderBlocks() {
	// Render blocks in array
	for (int i = 0; i < GRID_WIDTH; i++)
	{
		for (int j = 0; j < GRID_HEIGHT; j++)
		{
			if (gridBlock[i][j] != nullptr)
			{
				gridBlock[i][j]->render();
			}
		}
	}
	// Render controller blocks
	for (int i = 0; i < 4; i++)
	{
		if (controller.coords[i].getRow() < GRID_HEIGHT	&& controller.coords[i].getRow() >= 0 && controller.coords[i].getColumn() < GRID_WIDTH && controller.coords[i].getColumn() >= 0)
		{
			// if the block is in the screen
			controller.blocks[i]->render();
		}
	}
}

void updateGridBlocks() {
	for (int i = 0; i < GRID_WIDTH; i++)
	{
		for (int j = 0; j < GRID_HEIGHT; j++)
		{
			if (gridBlock[i][j] != nullptr && gridBlock[i][j]->getColorValue() == grid[i][j])
			{
				continue;
			}
			if (gridBlock[i][j] != nullptr && gridBlock[i][j]->getColorValue() != grid[i][j])
			{
				delete gridBlock[i][j];
				gridBlock[i][j] = new Block(i, j, grid[i][j]);
				continue;
			}
			if (gridBlock[i][j] == nullptr && grid[i][j] != 0)
			{
				gridBlock[i][j] = new Block(i, j, grid[i][j]);
				continue;
			}
		}
	}
}

void updateGrid() {

}

bool appendToGrid() {
	for (int i = 0; i < 4; i++)
	{
		// Check if out of bound, if so game over
		if (controller.coords[i].getColumn() >= GRID_WIDTH || controller.coords[i].getColumn() < 0 || controller.coords[i].getRow() >= GRID_HEIGHT || controller.coords[i].getRow() < 0)
		{
			return false;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		// Append coords from controller
		grid[controller.coords[i].getColumn()][controller.coords[i].getRow()] = controller.getColorValue();
	}
	return true;
}

bool checkLineFull(int row) {
	for (int i = 0; i < GRID_WIDTH; i++)
	{
		if (grid[i][row] == 0)
		{
			return false;
		}
	}
	return true;
}

void clearRow(int row) {
	for (int i = row; i < GRID_HEIGHT; i++)
	{

	}
}

void spawnBlock(int colorValue, int rotation) {
	rotation %= 4;
	int halfGridWidth = GRID_WIDTH / 2;
	// rotation may have 4 values max, 0, 1, 2, 3, depends on the shape
	// I(1) has 2
	// J(2) has 4
	// L(3) has 4
	// O(4) has none
	// S(5) has 2
	// T(6) has 4
	// Z(7) has 2
	switch (colorValue)
	{
	default:
		break;
	case 1:
		rotation %= 2;
		switch (rotation)
		{
		default:
			break;
		case 0:
			// Vertical I
			controller.setController(halfGridWidth, GRID_HEIGHT + 2, halfGridWidth, GRID_HEIGHT + 3, halfGridWidth, GRID_HEIGHT + 1, halfGridWidth, GRID_HEIGHT, colorValue);
			break;
		case 1:
			// Horizontal I
			controller.setController(halfGridWidth, GRID_HEIGHT, halfGridWidth - 1, GRID_HEIGHT, halfGridWidth + 1, GRID_HEIGHT, halfGridWidth + 2, GRID_HEIGHT, colorValue);
			break;
		}
		break;
	case 2:
		switch (rotation)
		{
		default:
			break;
		case 0:
			// Generic J
			controller.setController(halfGridWidth, GRID_HEIGHT + 1, halfGridWidth, GRID_HEIGHT + 2, halfGridWidth, GRID_HEIGHT, halfGridWidth - 1, GRID_HEIGHT, colorValue);
			break;
		case 1:
			// J rotated clockwise 90 degrees
			controller.setController(halfGridWidth, GRID_HEIGHT, halfGridWidth + 1, GRID_HEIGHT, halfGridWidth - 1, GRID_HEIGHT, halfGridWidth - 1, GRID_HEIGHT + 1, colorValue);
			break;
		case 2:
			// J rotated 180 degrees
			controller.setController(halfGridWidth, GRID_HEIGHT + 1, halfGridWidth, GRID_HEIGHT, halfGridWidth, GRID_HEIGHT + 2, halfGridWidth + 1, GRID_HEIGHT + 2, colorValue);
			break;
		case 3:
			// J rotated counter-clockwise 90 degrees
			controller.setController(halfGridWidth, GRID_HEIGHT + 1, halfGridWidth - 1, GRID_HEIGHT + 1, halfGridWidth + 1, GRID_HEIGHT + 1, halfGridWidth + 1, GRID_HEIGHT, colorValue);
			break;
		}
		break;
	case 3:
		switch (rotation)
		{
		default:
			break;
		case 0:
			// Generic L
			controller.setController(halfGridWidth, GRID_HEIGHT + 1, halfGridWidth, GRID_HEIGHT + 2, halfGridWidth, GRID_HEIGHT, halfGridWidth + 1, GRID_HEIGHT, colorValue);
			break;
		case 1:
			// L rotated clockwise 90 degrees
			controller.setController(halfGridWidth, GRID_HEIGHT + 1, halfGridWidth + 1, GRID_HEIGHT + 1, halfGridWidth - 1, GRID_HEIGHT + 1, halfGridWidth - 1, GRID_HEIGHT, colorValue);
			break;
		case 2:
			// L rotated 180 degrees
			controller.setController(halfGridWidth, GRID_HEIGHT + 1, halfGridWidth, GRID_HEIGHT, halfGridWidth, GRID_HEIGHT + 2, halfGridWidth - 1, GRID_HEIGHT + 2, colorValue);
			break;
		case 3:
			// L rotated counter-clockwise 90 degrees
			controller.setController(halfGridWidth, GRID_HEIGHT + 1, halfGridWidth - 1, GRID_HEIGHT + 1, halfGridWidth + 1, GRID_HEIGHT + 1, halfGridWidth + 1, GRID_HEIGHT + 2, colorValue);
		}
		break;
	case 4:
		// Generic O
		controller.setController(halfGridWidth, GRID_HEIGHT, halfGridWidth - 1, GRID_HEIGHT, halfGridWidth - 1, GRID_HEIGHT + 1, halfGridWidth, GRID_HEIGHT + 1, colorValue);
		break;
	case 5:
		rotation %= 2;
		switch (rotation)
		{
		default:
			break;
		case 0:
			// Vertical S
			controller.setController(halfGridWidth - 1, GRID_HEIGHT + 1, halfGridWidth - 1, GRID_HEIGHT + 2, halfGridWidth, GRID_HEIGHT + 1, halfGridWidth, GRID_HEIGHT, colorValue);
			break;
		case 1:
			// Flattened S
			controller.setController(halfGridWidth, GRID_HEIGHT + 1, halfGridWidth + 1, GRID_HEIGHT + 1, halfGridWidth, GRID_HEIGHT, halfGridWidth - 1, GRID_HEIGHT, colorValue);
		}
		break;
	case 6:
		switch (rotation)
		{
		default:
			break;
		case 0:
			// Generic T
			controller.setController(halfGridWidth, GRID_HEIGHT + 1, halfGridWidth - 1, GRID_HEIGHT + 1, halfGridWidth, GRID_HEIGHT, halfGridWidth + 1, GRID_HEIGHT + 1, colorValue);
			break;
		case 1:
			// T pointing to left
			controller.setController(halfGridWidth, GRID_HEIGHT + 1, halfGridWidth, GRID_HEIGHT + 2, halfGridWidth - 1, GRID_HEIGHT + 1, halfGridWidth, GRID_HEIGHT, colorValue);
			break;
		case 2:
			// Upside down T
			controller.setController(halfGridWidth, GRID_HEIGHT, halfGridWidth + 1, GRID_HEIGHT, halfGridWidth, GRID_HEIGHT + 1, halfGridWidth - 1, GRID_HEIGHT, colorValue);
			break;
		case 3:
			// T pointing to right
			controller.setController(halfGridWidth - 1, GRID_HEIGHT + 1, halfGridWidth - 1, GRID_HEIGHT, halfGridWidth, GRID_HEIGHT + 1, halfGridWidth - 1, GRID_HEIGHT + 2, colorValue);
		}
		break;
	case 7:
		rotation %= 2;
		switch (rotation)
		{
		default:
			break;
		case 0:
			// Vertical Z
			controller.setController(halfGridWidth, GRID_HEIGHT + 1, halfGridWidth, GRID_HEIGHT + 2, halfGridWidth - 1, GRID_HEIGHT + 1, halfGridWidth - 1, GRID_HEIGHT, colorValue);
			break;
		case 1:
			// Flattened Z
			controller.setController(halfGridWidth, GRID_HEIGHT + 1, halfGridWidth - 1, GRID_HEIGHT + 1, halfGridWidth, GRID_HEIGHT, halfGridWidth + 1, GRID_HEIGHT, colorValue);
			break;
		}
		break;
	}
}

int generateNextBlockColor() {
	srand(glfwGetTime()*10000000);
	// one of 1 to 7
	return rand() % 7 + 1;
}

int generateNextBlockRotation() {
	srand(glfwGetTime() * 10000000);
	// one of 0 to 3
	return rand() % 4;
}