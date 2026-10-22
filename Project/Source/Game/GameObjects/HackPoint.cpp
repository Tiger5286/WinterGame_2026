#include "HackPoint.h"
#include "Components/Hackable.h"
#include "System/EventManager.h"
#include "Components/Transform.h"

HackPoint::HackPoint()
{
	AddComponent<Hackable>();
}

HackPoint::~HackPoint()
{
}

void HackPoint::Init()
{
	GetComponent<Hackable>()->Init();
	Hackable::Info info;
	info.func = [this](Hackable::HackedData data) {OnHacked(); };
	GetComponent<Hackable>()->SetInfo(info);
}

void HackPoint::Update()
{
	auto hackable = GetComponent<Hackable>();
	if (hackable)
	{
		hackable->Update();
	}

	if (m_isRemovedHackable)
	{
		RemoveComponent<Hackable>();
	}
}

void HackPoint::Draw()
{
#ifdef _DEBUG
	DrawSphere3D(GetComponent<Transform>()->pos, 20, 16, 0x00ffff, 0xffffff, true);
#endif
}

void HackPoint::SetEvent(const std::wstring& event)
{
	m_eventName = event;
}

void HackPoint::OnHacked()
{
	EventManager::GetInstance().CallEvent(m_eventName);
	m_isRemovedHackable = true;
}