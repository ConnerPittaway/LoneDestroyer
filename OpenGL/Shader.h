#pragma once
#include <glad/include/glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	GLuint ID;

	Shader(const char* vertexPath, const char* fragmentPath)
	{
		//Stores code from the file
		std::string vertexCode;
		std::string fragmentCode;

		//The files
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		//Exceptions
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			vShaderFile.open(vertexPath); //Opens the vertex shader (.vs)
			fShaderFile.open(fragmentPath); //Opens the fragment shader (.fs)
			std::stringstream vShaderStream, fShaderStream;

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "Shader file read failed" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		//Compile Vertex and Fragment
		GLuint vertex, fragment;
		int success;
		char infoLog[512];

		//Compile Vertex
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);

		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "Vertex Compilation failed" << infoLog << std::endl;
		}

		//Compile Fragment
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "Fragment Compilation failed" << infoLog << std::endl;
		}

		//Shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);

		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "Link failed" << std::endl;
		}

		//Free memory
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	void use()
	{
		glUseProgram(ID);
	}

	//Uniform Setters
	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setMat4(const std::string& name, const glm::mat4& value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()),1, GL_FALSE, &value[0][0]);
	}
	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
	}
};
