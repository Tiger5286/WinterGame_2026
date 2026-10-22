#include "HackPoint.h"
#include "Components/Hackable.h"
#include "System/EventManager.h"

HackPoint::HackPoint()
{
	AddComponent<Hackable>();
}

HackPoint::~HackPoint()
{
}

void HackPoint::Init()
{
}

void HackPoint::Update()
{
}

void HackPoint::Draw()
{
}

void HackPoint::SetEvent(const std::wstring& event)
{
	m_eventName = event;
}

void HackPoint::OnHacked()
{
	EventManager::GetInstance().CallEvent(m_eventName);
}