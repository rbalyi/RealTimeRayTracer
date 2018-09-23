#include "../header/rectangle.h"
#include "../header/shaders.h"
#include "../header/globals.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL.h>

using namespace glm;
using namespace std;
using namespace Shaders;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

MyRectangle::MyRectangle() : m_uiVAO(0), m_uiVBO(0), m_uiEBO(0), m_iCount(6),
m_iShaderProgram(-1), m_iOrthoProjectionLocation(-1), m_iTextureDiffLocation(-1), m_uitextureID(0) {
	GLfloat vertices[] = {
		-1.f,  1.f, 0.0f,     0.0f, 0.0f,
		1.f, 1.f, 0.0f,     1.0f, 0.0f,
		1.f, -1.f, 0.0f,    1.0f, 1.0f,
		-1.f,  -1.f, 0.0f,    0.0f, 1.0f
	};
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	glGenVertexArrays(1, &m_uiVAO);
	glGenBuffers(1, &m_uiVBO);
	glGenBuffers(1, &m_uiEBO);
	glBindVertexArray(m_uiVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	string strVertexShaderSrc = LoadShaderSourceFromFile("..\\data\\shaders\\vertex\\vertex.shader");
	string strFragmentShaderSrc = LoadShaderSourceFromFile("..\\data\\shaders\\fragment\\fragment.shader");
	
	GLint iVertexShader = CompileShader(strVertexShaderSrc, SHADERIDS::SID_VERTEX);
	GLint iFragmentShader = CompileShader(strFragmentShaderSrc, SHADERIDS::SID_FRAGMENT);
	m_iShaderProgram = LinkProgram(iVertexShader, iFragmentShader);
	glUseProgram(0);
	InitTexture();
}

void MyRectangle::Render() {
	glUseProgram(m_iShaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uitextureID);
	glBindVertexArray(m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_iCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void MyRectangle::SetTextureData(unsigned char* pImage) {
	glBindTexture(GL_TEXTURE_2D, m_uitextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_iWidth, g_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void MyRectangle::InitTexture() {
	glGenTextures(1, &m_uitextureID);
	glBindTexture(GL_TEXTURE_2D, m_uitextureID);
	unsigned char* pImage = new unsigned char[g_iWidth * g_iHeight * 4];
	memset(pImage, 0, g_iWidth * g_iHeight * 4 * sizeof(unsigned char));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_iWidth, g_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] pImage;
	pImage = nullptr;
}