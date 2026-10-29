#include "WalkerStateDeath.h"
#include "Components/Animator/Animator.h"
#include "Components/Physics.h"
#include "../Walker.h"
#include "Components/Hackable.h"
#include "Components/Collider/CapsuleCollider.h"

WalkerStateDeath::WalkerStateDeath(Walker& walker) :
	WalkerState(walker)
{
}

WalkerStateDeath::~WalkerStateDeath()
{
}

void WalkerStateDeath::Enter()
{
	auto physics = m_owner.GetComponent<Physics>();
	physics->m_accel = Vector3::Zero();
	physics->m_vel = Vector3::Zero();
	// コンポーネントは削除せず無効化する(削除すると参照している箇所が壊れるため)
	m_owner.GetComponent<Hackable>()->SetEnable(false);			// ハッキング対象から外す
	m_owner.GetComponent<CapsuleCollider>()->SetEnable(false);	// プレイヤーがすり抜けられるようにする
	m_owner.GetComponent<Animator>()->Play(L"Walker|Death");
}

void WalkerStateDeath::Update()
{
	if (m_owner.GetComponent<Animator>()->IsEnd())
	{
		Die();
	}
}

void WalkerStateDeath::Exit()
{
}
