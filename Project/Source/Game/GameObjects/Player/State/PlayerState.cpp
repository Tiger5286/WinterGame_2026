#include "PlayerState.h"
#include "../Player.h"
#include "../../Camera/Camera.h"
#include "Components/Transform.h"
#include "Components/Physics.h"
#include "System/PadInput.h"
#include "Utility/Matrix4x4.h"
#include <cmath>

float PlayerState::GetCameraAngleY() const
{
	return m_owner.m_pCamera.lock()->GetComponent<Transform>()->rot.y;
}

void PlayerState::SetAngle(float angle)
{
	m_owner.m_angle = angle;
}

bool PlayerState::IsRun() const
{
	return m_owner.m_isRun;
}

void PlayerState::SetIsRun(bool isRun)
{
	m_owner.m_isRun = isRun;
}

void PlayerState::UpdateMove(float accel, float maxSpeed)
{
	auto physics = m_owner.GetComponent<Physics>();
	const Vector2 stick = PadInput::GetInstance().GetStickInput(PadInput::LR::Left);

	// カメラの向きを基準に入力方向をワールド座標へ変換する。
	const Matrix4x4 cameraRot = Matrix4x4::GetRotY(GetCameraAngleY());
	Vector3 moveDirection(stick.x, 0.0f, stick.y);
	moveDirection *= cameraRot;

	physics->SetMaxSpeed(maxSpeed);

	// 入力がない場合もゼロを代入し、前の加速度を残さない。
	physics->m_accel.x = moveDirection.x * accel;
	physics->m_accel.z = moveDirection.z * accel;

	// 入力があるときだけプレイヤーの向きを変更する。
	if (moveDirection.SquaredLength() > 0.0f)	
	{
		const float angle = std::atan2(-moveDirection.z, moveDirection.x) - DX_PI_F / 2;
		SetAngle(angle);
	}
}
