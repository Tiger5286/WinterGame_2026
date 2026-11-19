#pragma once
class Time
{
public:
	static Time& GetInstance();
	~Time() = default;

private:
	Time(const Time&) = delete;
	Time& operator=(const Time&) = delete;
	Time() = default;

	float GetTimeScale() const { return m_timeScale; }
	void SetTimeScale(float timeScale);

private:

	float m_timeScale = 1.0f;
};