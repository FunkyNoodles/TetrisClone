// System
#include <iostream>
#include <math.h>
#include <random>
#include <vector>
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
#include "resource.h"
#include "GameInfo.h"
#include "Block.h"
#include "Controller.h"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void clearGrid();
void renderBlocks();
void renderBlock(Block* block);
void updateGridBlocks();
bool appendToGrid();
bool checkRowFull(int row);
void removeRow(int row);
void spawnBlock(int colorValue, int rotation);
void blockDropped();
void fallBlocks();
int findNextFilledLine(int row);
void fillEmptySpots(int row);
int generateNextBlockColor();
int generateNextBlockRotation();
void initGridBlock();
void hardDropAppend();

// GL Variables
GLFWmonitor* monitor;
GLFWvidmode* desktopMode;

// Game Variables
bool fullscreen = false;

static Block* gridBlock[GRID_WIDTH][GRID_HEIGHT];

int grid[GRID_WIDTH][GRID_HEIGHT];

static Controller controller = Controller();
static GameInfo gameInfo = GameInfo();
Shader blockShader;
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
	Shader backColumnsShader = Shader(IDR_BACK_COLUMN_VERT, IDR_BACK_COLUMN_FRAG);
	blockShader = Shader(IDR_BLOCK_VERT, IDR_BLOCK_FRAG);
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
				backColumnVertices[6 * (4 * i + j) + 3] = 0.99; // color R
				backColumnVertices[6 * (4 * i + j) + 4] = 0.99; // color G
				backColumnVertices[6 * (4 * i + j) + 5] = 0.99; // color B
			}
			else
			{
				backColumnVertices[6 * (4 * i + j) + 3] = 0.8; // color R
				backColumnVertices[6 * (4 * i + j) + 4] = 0.8; // color G
				backColumnVertices[6 * (4 * i + j) + 5] = 0.8; // color B
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

	// Initialization
	initGridBlock();
	
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

	clearGrid();

	spawnBlock(generateNextBlockColor(), generateNextBlockRotation());

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		std::cout << gameInfo.getScore() << std::endl;

		// Render
		// Clear the colorbuffer
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		backColumnsShader.Use();

		// Draw back columns
		glBindVertexArray(backVAO);
		glDrawElements(GL_TRIANGLES, sizeof(backColumnsIndices)/sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		
		if (!gameInfo.isGamePaused())
		{
			if (gameInfo.isGameOver())
			{
				clearGrid();
				gameInfo.setGameOver(false);
				spawnBlock(generateNextBlockColor(), generateNextBlockRotation());
			}
			// Update controller
			gameInfo.updateElapsedTime(glfwGetTime());
			// If its time to move the block down
			if (gameInfo.getElapsedTime() > gameInfo.getDropSpeed())
			{
				// When the tetrimino can't move down
				if (!controller.canMoveDown(controller.coords))
				{
					blockDropped();
					if (gameInfo.isGameOver())
					{
						clearGrid();
					}
					spawnBlock(generateNextBlockColor(), generateNextBlockRotation());
					updateGridBlocks();
				}
				else {
					controller.moveDown(controller.coords);
					updateGridBlocks();
				}
				gameInfo.setPreviousTime(glfwGetTime());
			}
		}
		renderBlocks();

		if (gameInfo.isGamePaused())
		{
		}

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
	// ESC key
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
	// Test key
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		for (int i = 0; i < GRID_HEIGHT - 1; i++)
		{
			grid[0][i] = 3;
			grid[GRID_WIDTH - 1][i] = 3;
		}
	}
	// Move block down
	if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		if (!gameInfo.isGamePaused())
		{
			controller.moveDown(controller.coords);
			gameInfo.addScore(10);
		}
	}
	// Rotate block 90 degrees clockwise
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		if (!gameInfo.isGamePaused())
		{
			controller.rotate();
			controller.updateBlockProjection();
		}
	}
	// Move block left
	if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		if (!gameInfo.isGamePaused())
		{
			controller.moveLeft(controller.coords);
			controller.updateBlockProjection();
		}
	}
	// Move block right
	if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		if (!gameInfo.isGamePaused())
		{
			controller.moveRight(controller.coords);
			controller.updateBlockProjection();
		}
	}
	// Space bar to hard drop
	if (key == GLFW_KEY_SPACE && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		if (!gameInfo.isGamePaused())
		{
			hardDropAppend();
		}
	}
	// Pause or Unpause game
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		gameInfo.invertGamePaused();
	}
	// Restart the game
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		gameInfo.setScore(0);
		clearGrid();
		gameInfo.setGameOver(false);
		spawnBlock(generateNextBlockColor(), generateNextBlockRotation());
	}
}

// Clear grid data and render data
void clearGrid() {
	for (int i = 0; i < GRID_WIDTH; i++)
	{
		for (int j = 0; j < GRID_HEIGHT; j++)
		{
			grid[i][j] = 0;
			if (gridBlock[i][j] != nullptr)
			{
				delete gridBlock[i][j];
				gridBlock[i][j] = nullptr;
			}
		}
	}
}

// Render
void renderBlocks() {
	// Render blocks in array
	for (int i = 0; i < GRID_WIDTH; i++)
	{
		for (int j = 0; j < GRID_HEIGHT; j++)
		{
			if (gridBlock[i][j] != nullptr)
			{
				renderBlock(gridBlock[i][j]);
			}
		}
	}
	// Render projection blocks
	for (int i = 0; i < 4; i++)
	{
		renderBlock(controller.projectionBlocks[i]);
	}
	// Render controller blocks
	for (int i = 0; i < 4; i++)
	{	
		if (controller.coords[i].getRow() < GRID_HEIGHT	&& controller.coords[i].getRow() >= 0 && controller.coords[i].getColumn() < GRID_WIDTH && controller.coords[i].getColumn() >= 0)
		{
			// if the block is in the screen
			renderBlock(controller.blocks[i]);
		}
	}
}

// Render helper function
void renderBlock(Block* block) {
	blockShader.Use();
	glBindVertexArray(block->VAO);
	glDrawElements(GL_TRIANGLES, sizeof(block->indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

// tranfer color value from grid for rendering
void updateGridBlocks() {
	for (int i = 0; i < GRID_WIDTH; i++)
	{
		for (int j = 0; j < GRID_HEIGHT; j++)
		{
			if (gridBlock[i][j] != nullptr)
			{
				if (gridBlock[i][j]->getColorValue() == grid[i][j])
				{
					continue;
				}
				else
				{
					delete gridBlock[i][j];
					gridBlock[i][j] = nullptr;
					if (grid[i][j] > 0)
					{
						gridBlock[i][j] = new Block(i, j, grid[i][j]);
					}
					continue;
				}
			}
			if (gridBlock[i][j] == nullptr && grid[i][j] > 0)
			{
				gridBlock[i][j] = new Block(i, j, grid[i][j]);
				continue;
			}
		}
	}
}

// Transfer data from controller to grid, returns false if it a illegal append (i.e. coords are outisde the well)
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

// Check if a row needs removal
bool checkRowFull(int row) {
	for (int i = 0; i < GRID_WIDTH; i++)
	{
		if (grid[i][row] == 0)
		{
			return false;
		}
	}
	return true;
}

// Remove a row
void removeRow(int row) {
	for (int i = 0; i < GRID_WIDTH; i++)
	{
		grid[i][row] = -1;
	}
}

// Make all the blocks "fall" after line removals, a.k.a. shifting array down to fill all -1
void fallBlocks() {
	for (int j = 0; j < GRID_HEIGHT; j++)
	{
		if (grid[0][j] == -1)
		{
			int rowToCopyDown = findNextFilledLine(j);
			// If this is top most filled line
			if (rowToCopyDown == -1)
			{
				fillEmptySpots(j);
				return;
			}
			else
			{
				for (int i = 0; i < GRID_WIDTH; i++)
				{
					grid[i][j] = grid[i][rowToCopyDown]; // copy row from above
					grid[i][rowToCopyDown] = -1; // set above row to be empty
				}
			}
		}
	}
}

// Find next filled line above
int findNextFilledLine(int row) {
	for (int i = row + 1; i < GRID_HEIGHT; i++)
	{
		if (grid[0][i] != -1)
		{
			return i;
		}
	}
	return -1; // no lines above are filled
}

// Fill empty spots with 0 after line removal and falls
void fillEmptySpots(int row) {
	for (int i = 0; i < GRID_WIDTH; i++)
	{
		for (int j = row; j < GRID_HEIGHT; j++)
		{
			grid[i][j] = 0;
		}
	}
}

// When space bar is pressed, append block to its final position
void hardDropAppend() {
	int rowsSkipped = controller.appendDown();
	gameInfo.addScore(10 * rowsSkipped);
	gameInfo.setGameOver(!appendToGrid());
	// Decrement a previous time by a drop speed so a new block is guaranteed to spawn
	gameInfo.setPreviousTime(gameInfo.getPreviousTime() - gameInfo.getDropSpeed());
}

// Spawn a new block from the top and set controller to it
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
			break;
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
			break;
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
			break;
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
	controller.updateBlockProjection();
}

// Run to check when dropped
void blockDropped() {
	// Rows to check for completeness after block is set down
	std::vector<int> rowsToCheck;
	bool isRowCheckRepeat = false;
	rowsToCheck.clear();
	// Set up RowsToCheck
	for (int i = 0; i < 4; i++)
	{
		isRowCheckRepeat = false;
		for (int j = 0; j < (int)(rowsToCheck.size()); j++)
		{
			if (rowsToCheck[j] == controller.coords[i].getRow())
			{
				isRowCheckRepeat = true;
				break;
			}
		}
		if (!isRowCheckRepeat)
		{
			rowsToCheck.push_back(controller.coords[i].getRow());
		}
	}
	gameInfo.setGameOver(!appendToGrid());
	updateGridBlocks();

	// Remove rows if needed
	if (!rowsToCheck.empty())
	{
		// number of rows cleared
		int rowsRemoved = 0;
		
		for (int i = rowsToCheck.size() - 1; i >= 0; i--)
		{
			if (checkRowFull(rowsToCheck[i]))
			{
				removeRow(rowsToCheck[i]);
				rowsRemoved++;
			}
		}
		gameInfo.incrementStreak();
		// Score gets updated here as lines are being removed
		if (rowsRemoved == 4)
		{
			// a "Tetris", aka four lines get removed
			if (gameInfo.getLastPlaceBlockColorValue() == 1)
			{
				// Back to back "Tetris"
				gameInfo.addScore(400 * gameInfo.getStreak());
			}
			// Regular "Tetris"
			gameInfo.addScore(800 * gameInfo.getStreak());
		}
		else
		{
			// Regular scoring
			gameInfo.addScore(100 * rowsRemoved * gameInfo.getStreak());
		}
		fallBlocks();
		updateGridBlocks();
	}
	else {
		gameInfo.resetStreak();
	}
}

// Random generators
std::mt19937 rng;

// Generate a random color
int generateNextBlockColor() {
	// one of 1 to 7
	rng.seed(std::random_device()());
	std::uniform_int_distribution<> dist(1, 7);

	return dist(rng);
}

// Generate a random rotation
int generateNextBlockRotation() {
	// one of 0 to 3
	rng.seed(std::random_device()());
	std::uniform_int_distribution<> dist(0, 3);

	return dist(rng);
}

// Initialize grid block
void initGridBlock() {
	for (int i = 0; i < GRID_WIDTH; i++)
	{
		for (int j = 0; j < GRID_HEIGHT; j++)
		{
			gridBlock[i][j] = nullptr;
		}
	}
}