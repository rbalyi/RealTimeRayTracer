#include "../header/camera.h"
#include "../header/globals.h"

using namespace glm;

void Camera::HandleKeyboard(const GLfloat fDeltaTime) {
	if (g_bIsAnimationOn) {
		return;
	}
	GLfloat fVelocity = m_fSpeed * fDeltaTime;
	if (m_eDirection == DIRECTIONS::DIR_FORWARD) {
		m_vPos -= (m_vLookAt * fVelocity);
	}
	if (m_eDirection == DIRECTIONS::DIR_BACKWARD) {
		m_vPos += (m_vLookAt * fVelocity);
	}
	if (m_eDirection == DIRECTIONS::DIR_LEFT) {
		m_vPos += (m_vRight * fVelocity);
	}
	if (m_eDirection == DIRECTIONS::DIR_RIGHT) {
		m_vPos -= (m_vRight * fVelocity);
	}
}

void Camera::HandleMouse() {
	if (!m_bIsMouseInitialized) {
		m_fPrevMouseX = m_fCurrMouseX;
		m_fPrevMouseY = m_fCurrMouseY;
		m_bIsMouseInitialized = true;
	}

	m_fYaw -= (m_fCurrMouseX - m_fPrevMouseX) * m_fMouseSensitivity;
	m_fPitch -= (m_fPrevMouseY - m_fCurrMouseY) * m_fMouseSensitivity;
	m_fPrevMouseX = m_fCurrMouseX;
	m_fPrevMouseY = m_fCurrMouseY;

	if (m_fPitch > 89.0f) {
		m_fPitch = 89.0f;
	}
	if (m_fPitch < -89.0f) {
		m_fPitch = -89.0f;
	}

	vec3 vLookAt;
	vLookAt.x = cos(radians(m_fYaw)) * cos(radians(m_fPitch));
	vLookAt.y = sin(radians(m_fPitch));
	vLookAt.z = sin(radians(m_fYaw)) * cos(radians(m_fPitch));
	m_vLookAt = normalize(vec4(vLookAt.x, vLookAt.y, -vLookAt.z, 0.f)); //Note the negative z here!!!
	vec3 tmp = normalize(cross(vec3(m_vLookAt.x, m_vLookAt.y, m_vLookAt.z), vec3(m_vWorldUp.x, m_vWorldUp.y, m_vWorldUp.z)));
	m_vRight = vec4(tmp.x, tmp.y, tmp.z, 0.f);
	tmp = normalize(cross(vec3(m_vRight.x, m_vRight.y, m_vRight.z), vec3(m_vLookAt.x, m_vLookAt.y, m_vLookAt.z)));
	m_vUp = vec4(tmp.x, tmp.y, tmp.z, 0.f);
}