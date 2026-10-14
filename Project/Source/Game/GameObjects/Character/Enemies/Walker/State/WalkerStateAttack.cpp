#include "WalkerStateAttack.h"
#include "../Walker.h"
#include "Components/Animator/Animator.h"
#include "Components/State/StateMachine.h"
#include "WalkerStateApproach.h"
#include "Game/GameObjects/Attack.h"

namespace
{
	constexpr int kStartColFrame = 28 * 2;
	constexpr int kEndColFrame = 34 * 2;
}

WalkerStateAttack::WalkerStateAttack(Walker& walker) :
	WalkerState(walker)
{
}

WalkerStateAttack::~WalkerStateAttack()
{
}

void WalkerStateAttack::Enter()
{
	m_owner.GetComponent<Animator>()->Play(L"Armature|Attack");
}

void WalkerStateAttack::Update()
{
	m_frame++;
	if (m_frame == kStartColFrame)
	{
		m_pAtk = std::make_shared<Attack>();
		CollisionManager::AttackInfo info = {
			info.damage = 50
		};
		Attack::Data data = {
			.info = info,
			.colRadius = 100.0f
		};
		m_pAtk->SetData(data);
	}
	if (m_frame == kEndColFrame)
	{
		m_pAtk = nullptr;
	}
	if (m_owner.GetComponent<Animator>()->IsEnd())
	{
		m_owner.GetComponent<StateMachine<Walker>>()->ChangeState(std::make_unique<WalkerStateApproach>(m_owner));
		return;
	}
}

void WalkerStateAttack::Exit()
{
}

void WalkerStateAttack::Draw()
{
#ifdef _DEBUG
	if (m_pAtk)
	{
		m_pAtk->Draw();
	}
#endif
}
