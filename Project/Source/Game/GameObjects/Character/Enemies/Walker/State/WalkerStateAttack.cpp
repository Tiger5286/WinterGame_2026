#include "WalkerStateAttack.h"
#include "../Walker.h"
#include "Components/Animator/Animator.h"
#include "Components/State/StateMachine.h"
#include "WalkerStateOrbit.h"
#include "Game/GameObjects/Attack.h"
#include "System/ServiceLocator.h"
#include "Game/CollisionManager.h"
#include "Components/Physics.h"
#include "Components/Transform.h"
#include "Utility/Matrix4x4.h"

namespace
{
	constexpr int kStartColFrame = 28 * 2;
	constexpr int kEndColFrame = 34 * 2;
	constexpr int kEndAnimFrame = 95 * 2;

	const Vector3 kAttackOffset = { 0.0f,100.0f,-60.0f };

	constexpr int kAttackCooltime = 180;
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
	m_owner.GetComponent<Animator>()->Play(L"Walker|Attack");
	m_owner.GetComponent<Physics>()->m_accel = Vector3::Zero();
}

void WalkerStateAttack::Update()
{
	m_frame++;
	if (m_frame == kStartColFrame)
	{
		m_pAtk = std::make_shared<Attack>();
		CollisionManager::AttackInfo info = {
			.damage = 50
		};
		Attack::Data data = {
			.info = info,
			.colRadius = 60.0f,
			.hitObjTag = GameObject::Tag::Player
		};
		m_pAtk->SetData(data);
		m_pAtk->Init();

		auto transform = m_owner.GetComponent<Transform>();
		m_pAtk->GetComponent<Transform>()->pos = kAttackOffset * Matrix4x4::GetRotY(transform->rot.y) + transform->pos;

		ServiceLocator::GetInstance().GetCollisionManager().Register(m_pAtk);
	}
	if (m_frame == kEndColFrame)
	{
		m_pAtk = nullptr;
	}
	if (m_frame > kEndAnimFrame)
	{
		m_owner.GetComponent<StateMachine<Walker>>()->ChangeState(std::make_unique<WalkerStateOrbit>(m_owner));
		return;
	}
}

void WalkerStateAttack::Exit()
{
	SetAttackCooltime(kAttackCooltime);
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
