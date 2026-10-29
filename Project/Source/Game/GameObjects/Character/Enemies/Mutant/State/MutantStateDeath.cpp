#include "MutantStateDeath.h"
#include "../Mutant.h"
#include "Components/Animator/Animator.h"
#include "Components/Physics.h"
#include "Components/Collider/CapsuleCollider.h"

MutantStateDeath::MutantStateDeath(Mutant& mutant) :
	MutantState(mutant)
{
}

MutantStateDeath::~MutantStateDeath()
{
}

void MutantStateDeath::Enter()
{
	auto physics = m_owner.GetComponent<Physics>();
	physics->m_accel = Vector3::Zero();
	physics->m_vel = Vector3::Zero();
	// コンポーネントは削除せず無効化する(削除すると参照している箇所が壊れるため)
	m_owner.GetComponent<CapsuleCollider>()->SetEnable(false);	// プレイヤーがすり抜けられるようにする
	m_owner.GetComponent<Animator>()->Play(L"Mutant|Death");
}

void MutantStateDeath::Update()
{
	if (m_owner.GetComponent<Animator>()->IsEnd())
	{
		Die();
	}
}

void MutantStateDeath::Exit()
{
}
