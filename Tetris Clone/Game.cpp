// System
#include <iostream>
#include <math.h>
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

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void clearGrid();

// Windows
const int WIDTH = 800, HEIGHT = 600;

// GL Variables
GLFWmonitor* monitor;
GLFWvidmode* desktopMode;

// Game Variables
bool fullscreen = false;
const int TILE_WIDTH = HEIGHT / 20; // Tile width of a tile is demermined by window pixel height
const short GRID_WIDTH = 10;
const short GRID_HEIGHT = 20;
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
short grid[GRID_WIDTH][GRID_HEIGHT];

struct Block{
	GLdouble vertices[24]; // 3 pos 3 color
	GLuint indices[6];
	GLuint VAO, VBO, EBO;
	
	Block(int column, int row, int value) {
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
			switch (value)
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
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 1;
		indices[4] = 2;
		indices[5] = 3;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(GLdouble), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// Color attribute
		glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(GLdouble), (GLvoid*)(3 * sizeof(GLdouble)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
	}

	void render() {
		Shader blockShader("backColumnVertexShader.vert", "backColumnFragmentShader.frag");
		blockShader.Use();
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 4);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	~Block() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
};

Block* gridBlock[GRID_WIDTH][GRID_HEIGHT];

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
	GLdouble backColumnVertices[240]; // Format: 10 columns times four sets of three position and three color
	// Set up indices to draw back as rectangles
	GLuint backColumnsIndices[60]; //012,123,456,567, etc...

	//Cycle thru columns to fill backColumnVertices
	for (int i = 0; i < 10; i++)
	{
		//Cycle thru vertices
		for (int j = 0; j < 4; j++)
		{
			// Get pixel x and y, 0 is bottom left, 1 is bottom right, 2 is top left, 3 is top right
			switch (j)
			{
			case 0:
				pixelX = WIDTH - TILE_WIDTH*(10 - i);
				pixelY = 0;
				break;
			case 1:
				pixelX = WIDTH - TILE_WIDTH*(9 - i);
				pixelY = 0;
				break;
			case 2:
				pixelX = WIDTH - TILE_WIDTH*(10 - i);
				pixelY = HEIGHT;
				break;
			case 3:
				pixelX = WIDTH - TILE_WIDTH*(9 - i);
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
	}

	// Fill backColumnIndices
	for (int i = 0; i < 10; i++)
	{
		backColumnsIndices[6 * i] = 4 * i;
		backColumnsIndices[6 * i + 1] = 4 * i + 1;
		backColumnsIndices[6 * i + 2] = 4 * i + 2;
		backColumnsIndices[6 * i + 3] = 4 * i + 1;
		backColumnsIndices[6 * i + 4] = 4 * i + 2;
		backColumnsIndices[6 * i + 5] = 4 * i + 3;
	}

	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// Positions          // Colors           // Texture Coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Top Right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Bottom Right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Bottom Left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Top Left 
	};
	
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(backColumnVertices), backColumnVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backColumnsIndices), backColumnsIndices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(GLdouble), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(GLdouble), (GLvoid*)(3 * sizeof(GLdouble)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO


	// Texture 
	//int width, height;
	//unsigned char* image = SOIL_load_image("res/container.jpg", &width, &height, 0, SOIL_LOAD_RGB);

	////Texture 1
	//GLuint texture1;
	//glGenTextures(1, &texture1);
	//glBindTexture(GL_TEXTURE_2D, texture1);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//glGenerateMipmap(GL_TEXTURE_2D);
	//SOIL_free_image_data(image);
	//glBindTexture(GL_TEXTURE_2D, 0);

	////Texture 2
	//GLuint texture2;
	//image = SOIL_load_image("res/awesomeface.png", &width, &height, 0, SOIL_LOAD_RGB);
	//glGenTextures(1, &texture2);
	//glBindTexture(GL_TEXTURE_2D, texture2);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//glGenerateMipmap(GL_TEXTURE_2D);
	//SOIL_free_image_data(image);
	//glBindTexture(GL_TEXTURE_2D, 0);
	
	
	//trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	//trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

	GLuint transformLoc = glGetUniformLocation(backColumnsShader.Program, "transform");
	
	Block* testBlock = new Block(0, 0, 1);

	// Game loop
	//GLfloat timeValue, greenValue;
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		
		//glUniform4f(vertextColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
	/*	glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(shader.Program, "ourTexture1"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1i(glGetUniformLocation(shader.Program, "ourTexture2"), 1);*/

		// Activate shader
		backColumnsShader.Use();

		// Create transformation
		//glm::mat4 trans;
		//trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
		//trans = glm::rotate(trans, (GLfloat)glfwGetTime()*5.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		// Draw container
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, sizeof(backColumnsIndices)/sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		
		//std::cout << testBlock->vertices[0] << " " << testBlock->vertices[1] << " " << testBlock->vertices[2] << std::endl;
		testBlock->render();

		/*glBindVertexArray(testBlock->VAO);
		glDrawElements(GL_TRIANGLES, sizeof(testBlock->indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);*/
		//std::cout << testBlock->vertices[10] << std::endl;
		
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	delete testBlock;
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
	}
}

void clearGrid() {
	for (int i = 0; i < GRID_WIDTH; i++)
	{
		for (int j = 0; j < GRID_HEIGHT; j++)
		{
			grid[i][j] = 0;
			delete gridBlock[i][j];
		}
	}
}