#include "MutantStateApproach.h"
#include "../Mutant.h"
#include "MutantStateAttack.h"
#include "Components/Physics.h"
#include "Components/State/StateMachine.h"
#include "Components/Animator/Animator.h"

namespace
{
	constexpr float kMaxMoveSpeed = 2.0f;
}

MutantStateApproach::MutantStateApproach(Mutant& mutant) :
	MutantState(mutant)
{
}

MutantStateApproach::~MutantStateApproach()
{
}

void MutantStateApproach::Enter()
{
	m_owner.GetComponent<Physics>()->SetMaxSpeed(kMaxMoveSpeed);
	m_owner.GetComponent<Animator>()->Play(L"Mutant|WalkForward");
}

void MutantStateApproach::Update()
{
	RotToPlayer();

	Vector3 toPlayerXZ = GetToPlayerXZ();

	// 攻撃できる距離まで近づいたら攻撃
	if (toPlayerXZ.SquaredLength() < kAttackDist * kAttackDist)
	{
		m_owner.GetComponent<StateMachine<Mutant>>()->ChangeState(std::make_unique<MutantStateAttack>(m_owner));
		return;
	}

	toPlayerXZ.Normalize();
	m_owner.GetComponent<Physics>()->m_accel = toPlayerXZ * kAccel;
}

void MutantStateApproach::Exit()
{
	auto physics = m_owner.GetComponent<Physics>();
	physics->SetMaxSpeed(Physics::kDefaultMaxSpeed);
	physics->m_accel = Vector3::Zero();
}
