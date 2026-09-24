#include "Player.h"
#include "Resource/ResourceManager.h"
#include "System/PadInput.h"
#include "Game/GameObjects/Camera/Camera.h"

namespace
{
	constexpr float kMoveSpeed = 5.0f;
}

void Player::Init()
{
	m_pModel = ResourceManager::GetInstance().DuplicateModel(L"PlayerModel");
}

void Player::Update()
{
	Control();

	m_pModel->SetTransform(m_transform);
}

void Player::Draw() const
{
	m_pModel->Draw();
}

void Player::Control()
{
	// スティック入力を取得
	Vector2 stick = PadInput::GetInstance().GetStickInput(PadInput::LR::Left);

	// カメラの方向を入力と合成
	float cameraRotY = m_pCamera.lock()->GetTransform().rot.y;
	Matrix4x4 rot = Matrix4x4::GetRotY(cameraRotY);
	Vector3 stickVec3 = Vector3(stick.x, 0.0f, stick.y);
	stickVec3 *= rot;

	// 入力方向に移動
	m_transform.pos.x += stickVec3.x * kMoveSpeed;
	m_transform.pos.z += stickVec3.z * kMoveSpeed;

	// 入力があるときだけプレイヤーの向きを変える
	if (stickVec3.SquaredLength() > 0.0f)
	{
		float rot = atan2(-stickVec3.z, stickVec3.x) - DX_PI_F / 2;
		m_transform.rot.y = rot;
	}
}