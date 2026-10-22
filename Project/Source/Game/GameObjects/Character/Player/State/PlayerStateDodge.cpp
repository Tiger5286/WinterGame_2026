#include "PlayerStateDodge.h"
#include "PlayerStateMove.h"
#include "PlayerStateFall.h"
#include "PlayerStateHover.h"
#include "Components/Physics.h"
#include "../Player.h"
#include "Components/State/StateMachine.h"
#include "System/PadInput.h"
#include "Utility/Matrix4x4.h"

namespace
{
	constexpr float kMaxSpeed = 30.0f;
	constexpr float kMaxAccel = 10.0f;
	constexpr int kDodgeFrame = 20;

	constexpr float kHoverVelY = -0.5f;	// ホバリング中の目標Y速度
	constexpr float kHoverAccelY = 0.2f;	// ホバリング中に目標Y速度に近づけるための加速度
}

PlayerStateDodge::PlayerStateDodge(Player& player) :
	PlayerState(player)
{
}

PlayerStateDodge::~PlayerStateDodge()
{
}

void PlayerStateDodge::Enter()
{
	Vector2 stick = PadInput::GetInstance().GetStickInput(PadInput::LR::Left);
	if (stick.SquaredLength() > 0.0f)
	{
		Vector3 dir = Vector3(stick.x, 0.0f, stick.y);
		dir *= Matrix4x4::GetRotY(GetCameraAngleY());
		m_dir = dir.Normalized();
	}
	else
	{
		const float angle = GetAngle();
		Vector3 dir = Vector3(-sin(angle), 0.0f, -cos(angle));
		m_dir = dir.Normalized();
	}

	auto physics = m_owner.GetComponent<Physics>();
	physics->SetMaxSpeed(kMaxSpeed);

	m_owner.GetComponent<Physics>()->SetDrag(Physics::kDefaultDrag);

	KeepVelY();
}

void PlayerStateDodge::Update()
{
	auto& input = PadInput::GetInstance();

	m_frame++;
	auto physics = m_owner.GetComponent<Physics>();
	if (m_frame < 5)
	{
		physics->m_accel = m_dir * m_frame * 3;
	}
	else
	{
		physics->m_accel = Vector3::Zero();
	}

	KeepVelY();

	if (m_frame > kDodgeFrame)
	{
		if (physics->IsGrounded())
		{
			m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateMove>(m_owner));
			return;
		}
		else
		{
			if (input.IsPressed(XINPUT_BUTTON_A) || input.IsPressedTrigger(PadInput::LR::Left))
			{
				m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateHover>(m_owner));
				return;
			}
			else
			{
				m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateFall>(m_owner));
				return;
			}
		}
	}
}

void PlayerStateDodge::Exit()
{
	m_owner.GetComponent<Physics>()->SetGravity(Physics::kDefaultGravity);
	m_owner.GetComponent<Physics>()->m_accel.y = 0.0f;
}

void PlayerStateDodge::KeepVelY()
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