#include "Time.h"
#include <algorithm>
#include <cmath>

namespace
{
	constexpr float kSnapThreshold = 0.001f;
}

void Time::Update()
{
	if (m_keepFrame > 0)
	{
		m_keepFrame--;
		return;
	}

	if (m_isChangeSmooth)
	{
		m_timeScale = std::lerp(m_timeScale, m_targetTimeScale, m_lerpT);
		if (abs(m_targetTimeScale - m_timeScale) < kSnapThreshold)
		{
			m_isChangeSmooth = false;
			m_timeScale = m_targetTimeScale;
		}
	}
}

Time& Time::GetInstance()
{
	static Time instance;
	return instance;
}

void Time::SetTimeScale(float timeScale)
{
	if (timeScale < 0.0f)
	{
		timeScale = 0.0f;
	}
	m_timeScale = timeScale;
}

void Time::SetTimeScaleSmooth(float changeTimeScale, float targetTimeScale, float lerpT, int keepFrame)
{
	SetTimeScale(changeTimeScale);
	m_targetTimeScale = std::max(targetTimeScale, 0.0f);
	m_lerpT = lerpT;
	m_keepFrame = keepFrame;
	m_isChangeSmooth = true;
}