#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <GL/glew.h>

namespace Shaders {
	enum class SHADERIDS : int { SID_VERTEX = 0, SID_FRAGMENT = 1 };
	enum class SHADERORPROGRAM : int { T_SHADER = 0, T_PROGRAM = 1 };
	struct Shader {
		Shader() : m_uiShader(0) {}
		Shader(const GLuint uiShader) : m_uiShader(uiShader) {}
		~Shader() { m_uiShader = 0; }
		GLuint m_uiShader;
	};
	struct Program {
		Program() : m_uiProgram(0), m_uiVertexShader(0), m_uiFragmentShader(0) {}
		Program(const GLuint uiProgram, const GLuint uiVertexShader, const GLuint uiFragmentShader) : m_uiProgram(uiProgram), m_uiVertexShader(uiVertexShader), m_uiFragmentShader(uiFragmentShader) {}
		~Program() { m_uiProgram = m_uiVertexShader = m_uiFragmentShader = 0; }
		GLuint m_uiProgram;
		GLuint m_uiVertexShader;
		GLuint m_uiFragmentShader;
	};
	struct ErrorParam {
		ErrorParam(const SHADERORPROGRAM eType, const Shader& sShader) : m_eType(eType), m_sShader(sShader) {}
		ErrorParam(const SHADERORPROGRAM eType, const Program& sProgram) : m_eType(eType), m_sProgram(sProgram) {}
		~ErrorParam() {}
		SHADERORPROGRAM m_eType;
		union {
			Shader m_sShader;
			Program m_sProgram;
		};
	};

	std::string LoadShaderSourceFromFile(const char* pFileName);
	std::vector<GLchar> GetError(const ErrorParam& param);
	GLint CompileShader(const std::string& strShaderSrc, const SHADERIDS eShaderType);
	GLint LinkProgram(const GLuint &uiVertexShader, const GLuint &uiFragmentShader);
}