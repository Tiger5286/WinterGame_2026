#include "PlayerStateHover.h"
#include "System/PadInput.h"
#include "../Player.h"
#include "Components/Physics.h"
#include "Components/State/StateMachine.h"
#include "PlayerStateLand.h"
#include "PlayerStateFall.h"
#include "PlayerStateIdle.h"
#include "PlayerStateMove.h"
#include <memory>

namespace
{
	constexpr float kHoverVelY = -0.5f;	// ホバリング中の目標Y速度
	constexpr float kHoverAccelY = 0.2f;	// ホバリング中に目標Y速度に近づけるための加速度
}

void PlayerStateHover::Enter()
{
	m_owner.GetComponent<Physics>()->SetDrag(0.98f);
	KeepVelY();
}

void PlayerStateHover::Update()
{
	auto& input = PadInput::GetInstance();

	// 移動できるようにする
	UpdateMove(kHoverAccel, kMaxHoverSpeed);

	// 地面に着地したら着地ステートに遷移する
	if (m_owner.GetComponent<Physics>()->IsGrounded())
	{
		// エイムしている場合は着地ステートに遷移しない
		if (IsAim())
		{
			// 移動していたらMove, そうでなければIdleに遷移する
			if (input.GetStickInput(PadInput::LR::Left).SquaredLength() > 0.0f)
			{
				m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateMove>(m_owner));
				return;
			}
			else
			{
				m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateIdle>(m_owner));
				return;
			}
		}
		else
		{
			m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateLand>(m_owner));
			return;
		}
	}

	// エイムしている、もしくはジャンプボタンを押している間はホバリングする
	if (IsAim() || input.IsPressed(XINPUT_BUTTON_A))
	{
		KeepVelY();
	}
	else	// エイムもジャンプボタンも押していない場合は落下する
	{
		m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateFall>(m_owner));
	}
}

void PlayerStateHover::Exit()
{
	m_owner.GetComponent<Physics>()->SetGravity(Physics::kDefaultGravity);
	m_owner.GetComponent<Physics>()->m_accel.y = 0.0f;
}

void PlayerStateHover::KeepVelY()
{
	auto physics = m_owner.GetComponent<Physics>();

	// 目標速度との差を求める
	float velocityChange = kHoverVelY - physics->m_vel.y;

	// 1フレームの速度変化量を上下限に収め、目標を行き過ぎないようにする
	if (velocityChange > kHoverAccelY)
	{
		velocityChange = kHoverAccelY;
	}
	else if (velocityChange < -kHoverAccelY)
	{
		velocityChange = -kHoverAccelY;
	}

	// Physicsで後から加わる重力を差し引く
	physics->m_accel.y = velocityChange - physics->GetGravity();
}
