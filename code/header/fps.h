#pragma once

class FPS {
public:
	FPS() : m_uiFrameCount(0), m_uiFPSCount(0), m_fTime(0.0f) {}
	~FPS() {}
	void Update(const GLfloat fDeltaTime) {
		m_fTime += fDeltaTime;
		m_uiFrameCount++;
		if (m_fTime >= 1.f) {
			m_uiFPSCount = (unsigned int)(m_uiFrameCount / m_fTime);
			m_uiFrameCount = 0;
			m_fTime = 0.f;
		}
	}
	unsigned int GetFPS() const { return m_uiFPSCount;  }
private:
	unsigned int m_uiFrameCount;
	unsigned int m_uiFPSCount;
	GLfloat m_fTime;

	FPS(const FPS& other) = delete;
	FPS& operator=(FPS other) = delete;
};