#include "MutantStateInterval.h"
#include "../Mutant.h"
#include "MutantStateApproach.h"
#include "MutantStateAttack.h"
#include "Components/Physics.h"
#include "Components/State/StateMachine.h"
#include "Components/Animator/Animator.h"

MutantStateInterval::MutantStateInterval(Mutant& mutant) :
	MutantState(mutant)
{
}

MutantStateInterval::~MutantStateInterval()
{
}

void MutantStateInterval::Enter()
{
	m_owner.GetComponent<Physics>()->m_accel = Vector3::Zero();
	m_owner.GetComponent<Animator>()->Play(L"Mutant|Idle");
}

void MutantStateInterval::Update()
{
	RotToPlayer();

	// 攻撃のクールタイムが終わるまでその場で待つ
	if (GetAttackCooltime() > 0) return;

	auto stateMachine = m_owner.GetComponent<StateMachine<Mutant>>();
	const Vector3 toPlayerXZ = GetToPlayerXZ();
	if (toPlayerXZ.SquaredLength() < kAttackDist * kAttackDist)
	{
		stateMachine->ChangeState(std::make_unique<MutantStateAttack>(m_owner));
	}
	else
	{
		stateMachine->ChangeState(std::make_unique<MutantStateApproach>(m_owner));
	}
}

void MutantStateInterval::Exit()
{
}
