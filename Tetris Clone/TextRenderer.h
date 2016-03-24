#pragma once
// System
#include <map>

// GLEW
#define GLEW_STATIC
#include <glew.h>

// GLFW
#include <glfw3.h>

// GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

// Other includes 
#include "constants.h"
#include "Shader.h"
#include "resource.h"

struct Character
{
	GLuint TextureID; // ID handle of the glyph texture
	glm::ivec2 Size; // Size of glyph
	glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
	GLuint Advance; // Offset to advance to next glyph
};

class TextRenderer
{
public:
	TextRenderer();
	~TextRenderer();
	std::map<GLchar, Character> characters;
	Shader textShader;
	void load(std::string fontPath, GLuint fontSize);
	void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
private:
	GLuint textVAO, textVBO;
};

