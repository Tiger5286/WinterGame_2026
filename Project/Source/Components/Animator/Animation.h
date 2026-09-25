#pragma once

class Model;
class Handle;

class Animation
{
public:
	Animation(Model& model,int animIndex, float animSpeed = 0.5f,bool isLoop = true);
	~Animation() = default;

	int GetAnimIndex() const { return m_animIndex; }
	float GetAnimSpeed() const { return m_animSpeed; }
	bool IsLoop() const { return m_isLoop; }
	float GetAnimTotalTime() const { return m_totalTime; }
	int GetAttachIndex() const { return m_attachIndex; }

	void Attach();
	void Detach();

	void SetTime(float time);
	void SetBlendRate(float rate);


private:
	Model& m_model;
	int m_attachIndex = -1;
	int m_animIndex = -1;
	float m_animSpeed = 0.5f;
	bool m_isLoop = true;
	const int m_totalTime;
};

