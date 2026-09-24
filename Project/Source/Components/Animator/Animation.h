#pragma once
#include <memory>
#include <string>

class Model;
class Handle;

class Animation
{
public:
	Animation(int animIndex, float animSpeed = 0.5f,bool isLoop = true);
	~Animation() = default;

	int GetAnimIndex() const { return m_animIndex; }
	float GetAnimSpeed() const { return m_animSpeed; }
	bool IsLoop() const { return m_isLoop; }
	float GetAnimTotalTime(Model* pModel) const;

private:
	int m_animIndex = -1;
	int m_attachIndex = -1;
	float m_animSpeed = 0.5f;
	bool m_isLoop = true;
};

