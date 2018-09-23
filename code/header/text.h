#pragma once

#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "shaders.h"

struct sCharacter {
	GLuint m_uiTextureID;
	glm::ivec2 m_vSize;
	glm::ivec2 m_vBearing;
	GLuint m_uiAdvance;
};

class Text {
public:
	Text(const GLfloat fWidth, const GLfloat fHeight,
		const std::string& sFontFaceFileName = "..\\data\\fonts\\arial.ttf",
		const unsigned int uiFontSize = 48) : m_FreeTypeLib(nullptr), m_FontFace(nullptr), m_sFontFaceFileName(sFontFaceFileName), m_uiFontSize(uiFontSize), m_uiVAO(0), m_uiVBO(0), m_iShaderProgram(-1), m_iTextColorLocation(-1) {
		Init(fWidth, fHeight);
	}
	~Text() {}
	void RenderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
private:
	FT_Library m_FreeTypeLib;
	FT_Face m_FontFace;
	std::string m_sFontFaceFileName;
	unsigned int m_uiFontSize;
	std::map<GLchar, sCharacter> m_mapCharacters;
	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLint m_iShaderProgram;
	GLint m_iTextColorLocation;

	Text(const Text& other) = delete;
	Text& operator=(Text other) = delete;
	int InitFT();
	int DestroyFT() {
		FT_Done_Face(m_FontFace);
		FT_Done_FreeType(m_FreeTypeLib);
		return 0;
	}
	int Init(const GLfloat fWidth, const GLfloat fHeight);
	void InitVAOVBO();
	void InitShader(const GLfloat fWidth, const GLfloat fHeight);
};