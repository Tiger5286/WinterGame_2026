#include "MutantStateAttack.h"
#include "../Mutant.h"
#include "MutantStateInterval.h"
#include "Components/Animator/Animator.h"
#include "Components/State/StateMachine.h"
#include "Components/Physics.h"
#include "Components/Transform.h"
#include "Game/GameObjects/Attack.h"
#include "System/ServiceLocator.h"
#include "Game/CollisionManager.h"
#include "Utility/Matrix4x4.h"

namespace
{
	// 当たり判定を出すフレーム(アニメーションのフレーム×2)
	// TODO: Mutant|Attackの腕が当たるフレームに合わせて調整する
	constexpr int kStartColFrame = 30 * 2;
	constexpr int kEndColFrame = 40 * 2;

	// 攻撃を振り始めるまではプレイヤーの方を向き続ける
	constexpr int kEndRotFrame = 20 * 2;

	const Vector3 kAttackOffset = { 0.0f,150.0f,-200.0f };
	constexpr float kAttackRadius = 150.0f;
	constexpr int kAttackDamage = 100;

	constexpr int kAttackCooltime = 120;
}

MutantStateAttack::MutantStateAttack(Mutant& mutant) :
	MutantState(mutant)
{
}

MutantStateAttack::~MutantStateAttack()
{
}

void MutantStateAttack::Enter()
{
	m_owner.GetComponent<Animator>()->Play(L"Mutant|Attack");
	m_owner.GetComponent<Physics>()->m_accel = Vector3::Zero();
}

void MutantStateAttack::Update()
{
	m_frame++;
	if (m_frame < kEndRotFrame)
	{
		RotToPlayer();
	}
	if (m_frame == kStartColFrame)
	{
		m_pAtk = std::make_shared<Attack>();
		CollisionManager::AttackInfo info = {
			.damage = kAttackDamage
		};
		Attack::Data data = {
			.info = info,
			.colRadius = kAttackRadius,
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
	if (m_owner.GetComponent<Animator>()->IsEnd())
	{
		m_owner.GetComponent<StateMachine<Mutant>>()->ChangeState(std::make_unique<MutantStateInterval>(m_owner));
		return;
	}
}

void MutantStateAttack::Exit()
{
	m_pAtk = nullptr;
	SetAttackCooltime(kAttackCooltime);
}

void MutantStateAttack::Draw()
{
#ifdef _DEBUG
	if (m_pAtk)
	{
		m_pAtk->Draw();
	}
#endif
}
