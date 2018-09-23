#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <CL/cl.h>
#include "../header/globals.h"
#include "../header/fps.h"
#include "../header/text.h"
#include "../header/camera.h"
#include "../header/info.h"

using namespace glm;
using namespace std;
using namespace Info;

#define delPointer(ptr) delete (ptr); (ptr) = nullptr;
#define delArrayPointer(ptr) delete[] (ptr); (ptr) = nullptr;

GLFWwindow* g_pWindow = nullptr;
const int g_iWidth = 1024;
const int g_iHeight = 768;
const char* g_pTitle = "RealTimeRayTracing";
GLfloat g_fCurrentTime = 0.0f;
GLfloat g_fPreviousTime = 0.0f;
GLfloat g_fDeltaTime = 0.0f;
bool keys[1024];
Text* g_pText = nullptr;
FPS g_FPS;
cl_command_queue g_clQueue = nullptr;
cl_kernel g_clKernel = nullptr;
cl_mem g_clBuffer = nullptr;
cl_mem g_clViewTransform = nullptr;
cl_float4 g_clEyePos;
cl_float4 g_clLookAt;
cl_float4* g_pClptr = nullptr;
unsigned char* g_pPixels = new unsigned char[g_iWidth * g_iHeight * 4];
size_t g_Global_Work_Size = g_iWidth * g_iHeight;
MyRectangle* g_pRect = nullptr;
Camera g_Camera;
vec4 g_vCurrCamPos;
GLfloat g_fCameraCurrentTime = 0.f;
GLfloat g_fCameraStopTime = 10.f;
int g_iIndexInDirs = 0;
sCamMov g_Dirs[3];
bool g_bIsAnimationOn = true;

static void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (action == GLFW_PRESS) {
		keys[key] = true;
	} else if (action == GLFW_RELEASE) {
		keys[key] = false;
	}
	g_Camera.SetDirection(Camera::DIRECTIONS::DIR_NONE);
	if (keys[GLFW_KEY_W]) {
		g_Camera.SetDirection(Camera::DIRECTIONS::DIR_FORWARD);
	}
	if (keys[GLFW_KEY_S]) {
		g_Camera.SetDirection(Camera::DIRECTIONS::DIR_BACKWARD);
	}
	if (keys[GLFW_KEY_A]) {
		g_Camera.SetDirection(Camera::DIRECTIONS::DIR_LEFT);
	}
	if (keys[GLFW_KEY_D]) {
		g_Camera.SetDirection(Camera::DIRECTIONS::DIR_RIGHT);
	}
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (g_bIsAnimationOn) {
		return;
	}
	g_Camera.SetCurrMouseXY((GLfloat)xpos, (GLfloat)ypos);
}

GLFWwindow* CreateOpenGLWindow(const int iWidth, const int iHeight, const char* pTitle) {
	GLFWwindow* result = nullptr;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	result = glfwCreateWindow(iWidth, iHeight, pTitle, nullptr, nullptr);
	char *version = (char*)glGetString(GL_VERSION);
	if (!result) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(result);
	glfwSwapInterval(1);
	glfwSetKeyCallback(result, key_callback);
	glfwSetInputMode(result, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(result, mouse_callback);
	glViewport(0, 0, iWidth, iHeight);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	return result;
}

void PrintDeviceInfo(cl_device_id device, const cl_platform_id platform) {
	cl_uint units = 0;
	size_t workgroup = 0;
	PrintDeviceInfo(device);
	PrintPlatformInfo(platform);
	PrintError(clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(units), &units, nullptr), "clGetDeviceInfo");
	cout << "\nMax. compute units: " << units;
	PrintError(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workgroup), &workgroup, nullptr), "clGetDeviceInfo");
	cout << "\nMax. work group size: " << workgroup << endl;
	cout << "First there is an animated camera for about 40 seconds then it switches to free camera, use w, s, a, d + mouse to move\n around. Press any key ...";
	_getch();
}

bool InitOpenCL() {
	cl_platform_id platform;
	clGetPlatformIDs(1, &platform, nullptr);
	cl_device_id device;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);
	cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, nullptr);
	g_clQueue = clCreateCommandQueue(context, device, 0, nullptr);
	const char* pFileName = "..\\data\\kernel.cl";
	cout << "Opening file: " << pFileName << endl;
	ifstream file(pFileName);
	if (!file.is_open()) {
		cout << "Error reading file!!! Press a key to exit!" << endl;
		_getch();
		return false;
	}
	string source;
	while (!file.eof()) {
		char line[256];
		file.getline(line, 255);
		source += line;
		source += '\n';
	}
	cout << "File read. Building program..." << endl;
	const char* str = source.c_str();
	cl_program program = clCreateProgramWithSource(context, 1, &str, nullptr, nullptr);
	cl_int result = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
	if (result) {
		cout << "Error during compilation! (" << result << ")" << endl;
		size_t log_size;
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);
		char *log = (char *)malloc(log_size);
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, nullptr);
		cout << log;
		free(log);
		_getch();
		return false;
	} else {
		cout << "Program built." << endl;
	}
	g_clKernel = clCreateKernel(program, "render_kernel", nullptr);
	g_clBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, g_iWidth * g_iHeight * sizeof(cl_float4), nullptr, 0);
	clSetKernelArg(g_clKernel, 0, sizeof(g_clBuffer), (void*)&g_clBuffer);
	clSetKernelArg(g_clKernel, 1, sizeof(cl_uint), (void*)&g_iWidth);
	clSetKernelArg(g_clKernel, 2, sizeof(cl_uint), (void*)&g_iHeight);
	vec4 tmp = g_Camera.GetPosition();
	g_clEyePos.x = tmp.x;
	g_clEyePos.y = tmp.y;
	g_clEyePos.z = tmp.z;
	g_clEyePos.w = 1.f;
	clSetKernelArg(g_clKernel, 3, sizeof(cl_float4), (void*)&g_clEyePos);
	vec4 tmp2 = g_Camera.GetLookAt();
	g_clLookAt.x = tmp2.x;
	g_clLookAt.y = tmp2.y;
	g_clLookAt.z = tmp2.z;
	g_clLookAt.w = 0.f;
	clSetKernelArg(g_clKernel, 4, sizeof(cl_float4), (void*)&g_clLookAt);
	clReleaseProgram(program);
	clReleaseContext(context);
	PrintDeviceInfo(device, platform);
	return true;
}

void RayTrace() {
	vec4 pos = g_Camera.GetPosition();
	vec4 tmp = g_Camera.GetPosition();
	g_clEyePos.x = tmp.x;
	g_clEyePos.y = tmp.y;
	g_clEyePos.z = tmp.z;
	g_clEyePos.w = 1.f;
	clSetKernelArg(g_clKernel, 3, sizeof(cl_float4), (void*)&g_clEyePos);
	vec4 tmp2 = g_Camera.GetLookAt();
	g_clLookAt.x = tmp2.x;
	g_clLookAt.y = tmp2.y;
	g_clLookAt.z = tmp2.z;
	g_clLookAt.w = 0.f;
	clSetKernelArg(g_clKernel, 4, sizeof(cl_float4), (void*)&g_clLookAt);
	clEnqueueNDRangeKernel(g_clQueue, g_clKernel, 1, nullptr, &g_Global_Work_Size, nullptr, 0, nullptr, nullptr);
	g_pClptr = (cl_float4 *)clEnqueueMapBuffer(g_clQueue, g_clBuffer, CL_TRUE, CL_MAP_READ, 0, g_iWidth * g_iHeight * sizeof(cl_float4), 0, nullptr, nullptr, nullptr);
	memset(g_pPixels, 0, g_iWidth * g_iHeight * 4 * sizeof(unsigned char));
	for (int i = 0; i < g_iWidth * g_iHeight; i++) {
		g_pPixels[i * 4] = (g_pClptr[i].s[0] * 255);
		g_pPixels[i * 4 + 1] = (g_pClptr[i].s[1] * 255);
		g_pPixels[i * 4 + 2] = (g_pClptr[i].s[2] * 255);
		g_pPixels[i * 4 + 3] = 255;
	}
	clFinish(g_clQueue);

	if (g_pRect) {
		g_pRect->SetTextureData(g_pPixels);
	}
}

void SetDirs() {
	vec4 positions[] = { vec4(0.0f, 3.0f, 10.0f, 1.f), vec4(0.0f, 6.0f, 12.0f, 1.f), vec4(-30.0f, 6.0f, -16.0f, 1.f), vec4(-28.0f, 3.0f, -14.0f, 1.f) };
	g_vCurrCamPos = positions[0];
	g_Dirs[0] = sCamMov(normalize(positions[1] - positions[0]), length(positions[1] - positions[0]));
	g_Dirs[1] = sCamMov(normalize(positions[2] - positions[1]), length(positions[2] - positions[1]));
	g_Dirs[2] = sCamMov(normalize(positions[3] - positions[2]), length(positions[3] - positions[2]));
}

void CalcCameraMovement() {
	if (!g_bIsAnimationOn) {
		return;
	}
	g_fCameraCurrentTime += g_fDeltaTime;
	GLfloat magnitude = ((g_fCameraCurrentTime * 100.f) / g_fCameraStopTime) * 0.01f * g_Dirs[g_iIndexInDirs].m_fLength;
	vec4 tmp = g_vCurrCamPos + (magnitude * g_Dirs[g_iIndexInDirs].m_vDir);
	if (g_fCameraCurrentTime < g_fCameraStopTime) {
		g_Camera.SetPosition(tmp);
		if (g_iIndexInDirs == 1 && abs(g_Camera.GetYaw()) < 190.f) {
			g_Camera.SetYaw(g_Camera.GetYaw() - 0.5f);
		} else {
			GLfloat yaw = g_Camera.GetYaw();
			yaw = g_Camera.GetYaw();
		}
	} else {
		g_vCurrCamPos = g_Camera.GetPosition();
		g_fCameraCurrentTime = 0;
		if (g_iIndexInDirs < 3) {
			g_iIndexInDirs++;
		}
		if (g_iIndexInDirs >= 3) {
			g_bIsAnimationOn = false;
		}
	}
}

void Update() {
	CalcCameraMovement();
	g_Camera.Update(g_fDeltaTime);
	g_FPS.Update(g_fDeltaTime);
}

void Render() {
	RayTrace();
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (g_pRect) {
		g_pRect->Render();
	}
	if (g_pText) {
		g_pText->RenderText(to_string(g_FPS.GetFPS()), 25.0f, 25.0f, 1.0f, vec3(0.5, 0.8f, 0.2f));
	}
}

int main() {
	if (!InitOpenCL()) {
		exit(EXIT_FAILURE);
	}
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	g_pWindow = CreateOpenGLWindow(g_iWidth, g_iHeight, g_pTitle);
	glewExperimental = GL_TRUE;
	glewInit();
	g_pText = new Text(1024.0f, 768.0f);
	g_pRect = new MyRectangle();
	SetDirs();
	while (!glfwWindowShouldClose(g_pWindow)) {
		glfwPollEvents();
		g_fCurrentTime = (GLfloat)glfwGetTime();
		g_fDeltaTime = g_fCurrentTime - g_fPreviousTime;
		g_fPreviousTime = g_fCurrentTime;
		Update();
		Render();
		glfwSwapBuffers(g_pWindow);
		clFinish(g_clQueue);
	}
	delArrayPointer(g_pPixels);
	delPointer(g_pText);
	delPointer(g_pRect);
	clReleaseMemObject(g_clBuffer);
	clReleaseKernel(g_clKernel);
	clReleaseCommandQueue(g_clQueue);
	glfwDestroyWindow(g_pWindow);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}