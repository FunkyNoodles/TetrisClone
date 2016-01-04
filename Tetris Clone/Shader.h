#pragma once
#include <cstdlib>
#include <ctime>
#include <string>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <istream>
#include <iostream>
#include <Windows.h>
#include <algorithm>

#include <glew.h>

#include "resource.h"

class Shader
{
public:
	GLuint Program;
	// Constructor generates the shader
	Shader(const int vertexName, const int fragmentName)
	{
		// Load shaders
		DWORD vertSize = 0;
		DWORD fragSize = 0;

		const char* vertCode = NULL;
		const char* fragCode = NULL;

		loadFileFromResource(vertexName, TEXTFILE, vertSize, vertCode);
		loadFileFromResource(fragmentName, TEXTFILE, fragSize, fragCode);

		char* vertBuffer = new char[vertSize + 1];
		char* fragBuffer = new char[fragSize + 1];

		::memcpy(vertBuffer, vertCode, vertSize);
		::memcpy(fragBuffer, fragCode, fragSize);

		vertBuffer[vertSize] = 0;
		fragBuffer[fragSize] = 0;

		const GLchar* vShaderCode = (GLchar*)vertBuffer;
		const GLchar * fShaderCode = (GLchar*)fragBuffer;

		// Compile shaders
		GLuint vertex, fragment;
		GLint success;
		GLchar infoLog[512];
		// Vertex Shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		// Print compile errors if any
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// Fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		// Print compile errors if any
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// Shader Program
		this->Program = glCreateProgram();
		glAttachShader(this->Program, vertex);
		glAttachShader(this->Program, fragment);
		glLinkProgram(this->Program);
		// Print linking errors if any
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		// Delete the shaders as they're linked into the program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);

	}
	// Uses the current shader
	void Use()
	{
		glUseProgram(this->Program);
	}

	// load vertex and fragment shaders
	void loadFileFromResource(int name, int type, DWORD& size, const char*& data) {
		HMODULE handle = ::GetModuleHandle(NULL);
		HRSRC res = ::FindResource(handle, MAKEINTRESOURCE(name), MAKEINTRESOURCE(type));
		HGLOBAL resData = ::LoadResource(handle, res);
		size = ::SizeofResource(handle, res);
		data = static_cast<const char*>(::LockResource(resData));
	}
};

