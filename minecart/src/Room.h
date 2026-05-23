#pragma once

class Room {
public:
	Room();
	~Room();

	void draw(unsigned int shaderProgram, int modelloc);

private:
	unsigned int m_planeVAO, m_planeVBO;
	unsigned int m_floorTexture;

	unsigned int loadTexture(const char* path);
};