#include "MutantStateBackStep.h"
#include "../Mutant.h"
#include "Components/Physics.h"
#include "Components/State/StateMachine.h"
#include "MutantStateOrbit.h"
#include "MutantStateMissile.h"
#include "Components/Transform.h"

namespace
{
	// バックステップのフレーム数
	constexpr int kBackStepFrame = 45;

	constexpr float kBackStepMaxSpeed = 15.0f;
	constexpr float kBackStepAccel = 5.0f;
}

MutantStateBackStep::MutantStateBackStep(Mutant& mutant, NextState next) :
	MutantState(mutant),
	m_nextState(next)
{
}

MutantStateBackStep::~MutantStateBackStep()
{
}

void MutantStateBackStep::Enter()
{
	Vector3 toPlayerXZ = GetToPlayerXZ();
	m_backStepDir = -toPlayerXZ.Normalized();
	m_owner.GetComponent<Physics>()->SetMaxSpeed(kBackStepMaxSpeed);
}

void MutantStateBackStep::Update()
{
	m_frameCount++;

	// バックステップが終わったら次のステートに切り替える
	if (m_frameCount >= kBackStepFrame)
	{
		auto stateMachine = m_owner.GetComponent<StateMachine<Mutant>>();
		switch (m_nextState)
		{
		case NextState::Orbit:
			stateMachine->ChangeState(std::make_unique<MutantStateOrbit>(m_owner));
			break;
		case NextState::Missile:
			stateMachine->ChangeState(std::make_unique<MutantStateMissile>(m_owner));
			break;
		default:
			assert(false && "MutantStateBackStep::Update() : 次のステートが不正です");
			break;
		}
		return;
	}

	auto physics = m_owner.GetComponent<Physics>();
	// バックステップの移動処理
	// 前半は加速する
	if (m_frameCount < kBackStepFrame / 2)
	{
		physics->m_accel = m_backStepDir * kBackStepAccel;
	}
	else	// 後半は勝手に減速する
	{
		physics->m_accel = Vector3::Zero();
	}
}

void MutantStateBackStep::Exit()
{
	auto physics = m_owner.GetComponent<Physics>();
	physics->SetMaxSpeed(Physics::kDefaultMaxSpeed);
	physics->m_accel = Vector3::Zero();
}
