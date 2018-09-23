#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class MyRectangle {
public:
	MyRectangle();
	~MyRectangle() {
		glDeleteVertexArrays(1, &m_uiVAO);
		glDeleteBuffers(1, &m_uiVBO);
		glDeleteBuffers(1, &m_uiEBO);
		glDeleteTextures(1, &m_uitextureID);
	}
	void Render();
	void SetTextureData(unsigned char* pImage);
protected:
	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLuint m_uiEBO;
	GLsizei m_iCount;
	GLint m_iShaderProgram;
	GLint m_iOrthoProjectionLocation;
	GLint m_iTextureDiffLocation;
	glm::mat4 m_mOrthoProjection;
	GLuint m_uitextureID = 0;

	MyRectangle(const MyRectangle& other) = delete;
	MyRectangle& operator=(MyRectangle other) = delete;
	void InitTexture();
};