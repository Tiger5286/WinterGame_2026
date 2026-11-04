#include "MutantStateOrbit.h"
#include "../Mutant.h"
#include "MutantStateApproach.h"
#include "MutantStateAttack.h"
#include "Components/Physics.h"
#include "Components/State/StateMachine.h"
#include "Components/Animator/Animator.h"

namespace
{
	constexpr float kMaxMoveSpeed = 1.0f;

	// この距離より離れたら周回をやめて接近する
	constexpr float kChangeApproachDist = 600.0f;

	constexpr float kOrbitDistance = 300.0f;	// 維持したい距離
	constexpr float kOrbitSpeed = 0.8f;			// 横移動の速度
	constexpr float kDistanceGain = 0.05f;		// 距離を戻す強さ
	constexpr float kMaxCorrectSpeed = 0.5f;	// 距離補正の最高速度
	constexpr float kVelocityGain = 0.2f;		// 目標速度へ近づく強さ
}

MutantStateOrbit::MutantStateOrbit(Mutant& mutant) :
	MutantState(mutant)
{
}

MutantStateOrbit::~MutantStateOrbit()
{
}

void MutantStateOrbit::Enter()
{
	m_owner.GetComponent<Physics>()->SetMaxSpeed(kMaxMoveSpeed);
	// 横歩きのアニメーションがないため仮で前歩きを使う
	m_owner.GetComponent<Animator>()->Play(L"Mutant|WalkForward");
}

void MutantStateOrbit::Update()
{
	RotToPlayer();

	auto stateMachine = m_owner.GetComponent<StateMachine<Mutant>>();
	const Vector3 toPlayerXZ = GetToPlayerXZ();

	// 攻撃のクールタイムが終わったら、距離に応じて攻撃か接近をする
	if (GetAttackCooltime() <= 0)
	{
		if (toPlayerXZ.SquaredLength() < kAttackDist * kAttackDist)
		{
			stateMachine->ChangeState(std::make_unique<MutantStateAttack>(m_owner));
		}
		else
		{
			stateMachine->ChangeState(std::make_unique<MutantStateApproach>(m_owner));
		}
		return;
	}

	// プレイヤーとの水平距離
	const float distance = toPlayerXZ.Length();
	if (distance < 0.001f)
	{
		return; // 同じ位置では方向を計算できない
	}

	// プレイヤーへ向かう方向
	const Vector3 toPlayer = toPlayerXZ / distance;

	// 周回方向
	const Vector3 tangent = toPlayer.Cross(Vector3::Up());

	// 遠ければプレイヤーへ、近ければ離れる方向へ補正する
	float correctSpeed = (distance - kOrbitDistance) * kDistanceGain;
	if (correctSpeed > kMaxCorrectSpeed)
	{
		correctSpeed = kMaxCorrectSpeed;
	}
	else if (correctSpeed < -kMaxCorrectSpeed)
	{
		correctSpeed = -kMaxCorrectSpeed;
	}

	// 横移動と距離補正を合わせた目標速度
	const Vector3 targetVelocity = tangent * kOrbitSpeed + toPlayer * correctSpeed;

	// 現在の水平速度を目標速度へ近づける
	auto physics = m_owner.GetComponent<Physics>();
	const Vector3 currentVelocity(physics->m_vel.x, 0.0f, physics->m_vel.z);
	physics->m_accel = (targetVelocity - currentVelocity) * kVelocityGain;
}

void MutantStateOrbit::Exit()
{
	auto physics = m_owner.GetComponent<Physics>();
	physics->SetMaxSpeed(Physics::kDefaultMaxSpeed);
	physics->m_accel = Vector3::Zero();
}
