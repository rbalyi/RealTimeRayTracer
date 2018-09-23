#include "../header/text.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;
using namespace glm;
using namespace Shaders;

void Text::RenderText(const string& text, GLfloat x, GLfloat y, GLfloat scale, vec3 color) {
	glUseProgram(m_iShaderProgram);
	glUniform3f(m_iTextColorLocation, color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(m_uiVAO);

	for (string::const_iterator cit = text.begin(); cit != text.end(); cit++) {
		sCharacter ch = m_mapCharacters[*cit];

		GLfloat xpos = x + ch.m_vBearing.x * scale;
		GLfloat ypos = y - (ch.m_vSize.y - ch.m_vBearing.y) * scale;

		GLfloat w = ch.m_vSize.x * scale;
		GLfloat h = ch.m_vSize.y * scale;
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		glBindTexture(GL_TEXTURE_2D, ch.m_uiTextureID);
		glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.m_uiAdvance >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

int Text::InitFT() {
	if (FT_Init_FreeType(&m_FreeTypeLib)) {
		throw std::runtime_error("Error: FreeType library cannot be initialized!");
		return 0;
	}

	if (FT_New_Face(m_FreeTypeLib, m_sFontFaceFileName.c_str(), 0, &m_FontFace)) {
		throw std::runtime_error("Error: Font cannot be loaded!");
		return 0;
	}

	return FT_Set_Pixel_Sizes(m_FontFace, 0, m_uiFontSize);
}

int Text::Init(const GLfloat fWidth, const GLfloat fHeight) {
	if (!InitFT()) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		FT_Bitmap* pFontBitmap = nullptr;
		for (GLubyte c = 0; c < 128; c++) {
			if (FT_Load_Char(m_FontFace, c, FT_LOAD_RENDER)) {
				continue;
			}
			pFontBitmap = &m_FontFace->glyph->bitmap;
			GLuint uiTexture;
			glGenTextures(1, &uiTexture);
			glBindTexture(GL_TEXTURE_2D, uiTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pFontBitmap->width, pFontBitmap->rows, 0, GL_RED, GL_UNSIGNED_BYTE, pFontBitmap->buffer);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			sCharacter character = { uiTexture, ivec2(pFontBitmap->width, pFontBitmap->rows), ivec2(m_FontFace->glyph->bitmap_left, m_FontFace->glyph->bitmap_top), (GLuint)(m_FontFace->glyph->advance.x) };
			m_mapCharacters.insert(pair<GLchar, sCharacter>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		DestroyFT();
		InitVAOVBO();
		InitShader(fWidth, fHeight);
		return 0;
	}
	return 1;
}

void Text::InitVAOVBO() {
	glGenVertexArrays(1, &m_uiVAO);
	glGenBuffers(1, &m_uiVBO);
	glBindVertexArray(m_uiVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Text::InitShader(const GLfloat fWidth, const GLfloat fHeight) {
	string strVertexShaderSrc = LoadShaderSourceFromFile("..\\data\\shaders\\vertex\\textvertex.shader");
	string strFragmentShaderSrc = LoadShaderSourceFromFile("..\\data\\shaders\\fragment\\textfragment.shader");
	GLint iVertexShader = CompileShader(strVertexShaderSrc, SHADERIDS::SID_VERTEX);
	GLint iFragmentShader = CompileShader(strFragmentShaderSrc, SHADERIDS::SID_FRAGMENT);
	m_iShaderProgram = LinkProgram(iVertexShader, iFragmentShader);
	m_iTextColorLocation = glGetUniformLocation(m_iShaderProgram, "uvTextColor");
	mat4 mProjection = ortho(0.0f, fWidth, 0.0f, fHeight);
	glUseProgram(m_iShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(m_iShaderProgram, "umProjection"), 1, GL_FALSE, value_ptr(mProjection));
	glUseProgram(0);
}