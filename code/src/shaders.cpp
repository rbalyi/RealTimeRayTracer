#include "../header/shaders.h"

using namespace std;

string Shaders::LoadShaderSourceFromFile(const char* pFileName) {
	ifstream isFile(pFileName);
	stringstream buffer;
	if (isFile.is_open()) {
		buffer << isFile.rdbuf();
	}
	return buffer.str();
}

vector<GLchar> Shaders::GetError(const ErrorParam& param) {
	GLint iMaxLength = 512;
	glGetShaderiv(param.m_eType == SHADERORPROGRAM::T_SHADER ? param.m_sShader.m_uiShader : param.m_sProgram.m_uiProgram, GL_INFO_LOG_LENGTH, &iMaxLength);
	if (iMaxLength == 0) {
		iMaxLength = 512;
	}
	vector<GLchar> result(iMaxLength);
	if (param.m_eType == SHADERORPROGRAM::T_SHADER) {
		glGetShaderInfoLog(param.m_sShader.m_uiShader, iMaxLength, &iMaxLength, &result[0]);
		glDeleteShader(param.m_sShader.m_uiShader);
	} else {
		glGetProgramInfoLog(param.m_sProgram.m_uiProgram, iMaxLength, &iMaxLength, &result[0]);
		glDeleteProgram(param.m_sProgram.m_uiProgram);
		glDeleteShader(param.m_sProgram.m_uiVertexShader);
		glDeleteShader(param.m_sProgram.m_uiFragmentShader);
	}
	return result;
}

GLint Shaders::CompileShader(const string& strShaderSrc, const SHADERIDS eShaderType) {
	GLuint iResult = 0;
	iResult = glCreateShader(eShaderType == SHADERIDS::SID_VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	const GLchar *source = (const GLchar *)strShaderSrc.c_str();
	glShaderSource(iResult, 1, &source, 0);
	glCompileShader(iResult);
	GLint iIsCompiled = 0;
	glGetShaderiv(iResult, GL_COMPILE_STATUS, &iIsCompiled);
	if (iIsCompiled == GL_FALSE) {
		//vector<GLchar> vcInfoLog = GetError(ErrorParam(SHADERORPROGRAM::T_SHADER, Shader(iResult)));
		return 0;
	}
	return iResult;
}

GLint Shaders::LinkProgram(const GLuint &uiVertexShader, const GLuint &uiFragmentShader) {
	GLuint uiResult = 0;
	uiResult = glCreateProgram();
	glAttachShader(uiResult, uiVertexShader);
	glAttachShader(uiResult, uiFragmentShader);
	glLinkProgram(uiResult);
	GLint iIsCompiled = 0;
	glGetProgramiv(uiResult, GL_LINK_STATUS, (int *)&iIsCompiled);
	if (iIsCompiled == GL_FALSE) {
		//vector<GLchar> vcInfoLog = GetError(ErrorParam(SHADERORPROGRAM::T_PROGRAM, Program(uiResult, uiVertexShader, uiFragmentShader)));
		return 0;
	}
	glDeleteShader(uiVertexShader);
	glDeleteShader(uiFragmentShader);
	return uiResult;
}