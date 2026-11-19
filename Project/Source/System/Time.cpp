#include "Time.h"

void Time::SetTimeScale(float timeScale)
{
	if (timeScale < 0.0f)
	{
		timeScale = 0.0f;
	}
	m_timeScale = timeScale;
}
