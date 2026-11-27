#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Hackable.h"
#include "System/Time.h"

GameObject::GameObject()
{
	m_transform = AddComponent<Transform>();
}

GameObject::~GameObject()
{
}

bool GameObject::IsHackable()
{
	auto hackable = GetComponent<Hackable>();
	// 無効化されているものはハッキング対象にしない
	return hackable && hackable->IsEnable();
}

float GameObject::GetTimeScale() const
{
	return Time::GetInstance().GetTimeScale() * m_timeScale;
}

void GameObject::SetTimeScale(float timeScale)
{
	if (timeScale < 0)
	{
		timeScale = 0;
	}
	m_timeScale = timeScale;
}