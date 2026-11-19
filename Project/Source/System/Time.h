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

public:

	void Update();

	float GetTimeScale() const { return m_timeScale; }
	void SetTimeScale(float timeScale);

	/// <summary>
	/// スムーズにタイムスケールを切り替える
	/// </summary>
	/// <param name="changeTimeScale">この関数を呼んだ瞬間に切り替わるタイムスケール</param>
	/// <param name="targetTimeScale">徐々に切り替わっていくタイムスケール</param>
	/// <param name="lerpT">スムーズに切り替えるlerp処理の係数</param>
	/// <param name="keepFrame">切り替わる前のタイムスケールを何フレーム持続させるか</param>
	void SetTimeScaleSmooth(float changeTimeScale, float targetTimeScale, float lerpT, int keepFrame);

private:
	float m_timeScale = 1.0f;

	bool m_isChangeSmooth = false;
	float m_targetTimeScale = 1.0f;
	float m_lerpT = 0.5f;
	int m_keepFrame = 0;
};