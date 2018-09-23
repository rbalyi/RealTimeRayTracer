#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <CL/cl.h>
#include "fps.h"
#include "text.h"
#include "rectangle.h"

struct sCamMov {
	sCamMov() : m_fLength(0.f) {}
	sCamMov(const glm::vec4& dir, const GLfloat& fLength) : m_vDir(dir), m_fLength(fLength) {}
	glm::vec4 m_vDir;
	GLfloat m_fLength;
};

extern GLFWwindow* g_pWindow;
extern const int g_iWidth;
extern const int g_iHeight;
extern const char* g_pTitle;
extern GLfloat g_fCurrentTime;
extern GLfloat g_fPreviousTime;
extern GLfloat g_fDeltaTime;
extern bool keys[1024];
extern Text* g_pText;
extern FPS g_FPS;
extern cl_command_queue g_clQueue;
extern cl_kernel g_clKernel;
extern cl_mem g_clBuffer;
extern unsigned char* g_pPixels;
extern size_t g_Global_Work_Size;
extern MyRectangle* g_pRect;
extern glm::vec4 g_vCurrCamPos;
extern GLfloat g_fCameraCurrentTime;
extern GLfloat g_fCameraStopTime;
extern int g_iIndexInDirs;
extern sCamMov g_Dirs[3];
extern bool g_bIsAnimationOn;