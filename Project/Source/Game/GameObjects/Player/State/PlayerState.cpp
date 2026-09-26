#include "PlayerState.h"
#include "../Player.h"
#include "../../Camera/Camera.h"
#include "Components/Transform.h"

float PlayerState::GetCameraAngleY() const
{
	return m_owner.m_pCamera.lock()->GetComponent<Transform>()->rot.y;
}

void PlayerState::SetAngle(float angle)
{
	m_owner.m_angle = angle;
}