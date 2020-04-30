#pragma once
#include <GLFW/glfw3.h>
#include <glad/include/glad/glad.h>
#include <vector>
#include <string>

class Texture2D
{
private:
	GLuint _ID;
	int _width, _height, _nrChannels;

public:
	Texture2D();
	~Texture2D();
	bool Load(char* path, bool flip);
	bool LoadPNG(char* path, bool flip);
	bool LoadCubeMap(std::vector<std::string> faces);

	GLuint GetID() const { return _ID; }
	int GetWidth() const { return _width; }
	int GetHeight() const { return _height;}
};

