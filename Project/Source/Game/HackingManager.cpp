#include "HackingManager.h"
#include "Game/GameObjects/GameObject.h"
#include "Components/Hackable.h"

HackingManager::HackingManager()
{
}

HackingManager::~HackingManager()
{
}

void HackingManager::Init(const std::shared_ptr<Player>& pPlayer)
{
	m_pPlayer = pPlayer;
}

void HackingManager::Update()
{
	if (!m_pHackingObject.lock()) return;
	auto hackable = m_pHackingObject.lock()->GetComponent<Hackable>();
	if (!hackable) return;
	hackable->Update();
}

void HackingManager::Draw()
{
	if (!m_pHackingObject.lock()) return;
	auto hackable = m_pHackingObject.lock()->GetComponent<Hackable>();
	if (!hackable) return;
	hackable->Draw();
}