#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Windows.h>
#include <sstream>

class Camera {
public:
	enum class DIRECTIONS : int { DIR_NONE = -1, DIR_FORWARD = 0, DIR_BACKWARD = 1, DIR_LEFT = 2, DIR_RIGHT = 3 };
	Camera() : m_vPos(0.0f, 3.0f, 10.0f, 1.f), m_vLookAt(0.0f, 0.0f, -1.0f, 0.f), m_vUp(0.0f, 1.0f, 0.0f, 0.f),
		m_vWorldUp(0.0f, 1.0f, 0.0f, 0.f), m_vRight(1.0f, 0.0f, 0.0f, 0.f), m_bIsMouseInitialized(false),
		m_fYaw(-90.0f), m_fPitch(0.0f), m_fSpeed(2.f), m_fMouseSensitivity(0.02f),
		m_fCurrMouseX(0.0f), m_fCurrMouseY(0.0f), m_fPrevMouseX(0.0f), m_fPrevMouseY(0.0f),
		m_eDirection(DIRECTIONS::DIR_NONE) {};
	~Camera() {};
	void Update(const float fDeltaTime) {
		HandleKeyboard(fDeltaTime);
		HandleMouse();
	}
	void SetDirection(const DIRECTIONS dir) {
		m_eDirection = dir;
	}
	glm::vec4 GetPosition() const {
		return m_vPos;
	}
	void SetPosition(const glm::vec4 vPos) {
		m_vPos = vPos;
	}
	GLfloat GetPitch() const {
		return m_fPitch;
	}
	void SetPitch(const GLfloat fPitch) {
		m_fPitch = fPitch;
	}
	GLfloat GetYaw() const {
		return m_fYaw;
	}
	void SetYaw(const GLfloat fYaw) {
		m_fYaw = fYaw;
	}
	void SetCurrMouseXY(const GLfloat fCurrMouseX, const GLfloat fCurrMouseY) {
		m_fCurrMouseX = fCurrMouseX;
		m_fCurrMouseY = fCurrMouseY;
	}
	glm::vec4 GetLookAt() const {
		return m_vLookAt;
	}
private:
	glm::vec4 m_vPos;
	glm::vec4 m_vLookAt;
	glm::vec4 m_vUp;
	glm::vec4 m_vWorldUp;
	glm::vec4 m_vRight;
	bool m_bIsMouseInitialized;
	GLfloat m_fYaw;
	GLfloat m_fPitch;
	GLfloat m_fSpeed;
	GLfloat m_fMouseSensitivity;
	GLfloat m_fCurrMouseX;
	GLfloat m_fCurrMouseY;
	GLfloat m_fPrevMouseX;
	GLfloat m_fPrevMouseY;
	DIRECTIONS m_eDirection;

	Camera(const Camera& other) = delete;
	Camera& operator=(Camera other) = delete;
	void HandleKeyboard(const GLfloat fDeltaTime);
	void HandleMouse();
};