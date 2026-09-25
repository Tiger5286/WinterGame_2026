#include "Camera.h"
#include "Game/GameObjects/Player/Player.h"
#include "Utility/Matrix4x4.h"
#include "System/PadInput.h"
#include <algorithm>
#include "Components/Transform.h"
#include <cmath>

namespace
{
	constexpr float kDefaultFov = DX_PI_F / 3;
	constexpr float kAimFov = DX_PI_F / 4;

	constexpr float kDefaultDist = 150.0f;
	const Vector3 kPosOffset = Vector3(70.0f, 150.0f,0.0f);

	constexpr float kSensitivity = DX_PI_F / 60;

	constexpr float kRotXLimit = DX_PI_F / 2 - 0.01f;
}

void Camera::Init()
{
	Transform& transform = *GetComponent<Transform>();
	transform.pos = Vector3(0, 0, 0);
	SetCameraPositionAndTarget_UpVecY(transform.pos, m_targetPos);
	SetupCamera_Perspective(kDefaultFov);
}

void Camera::Update()
{
	Transform& transform = *GetComponent<Transform>();
	// 入力を反映
	Control();
	// 回転の数値を調整
	// X軸回転の上限下限を設定
	transform.rot.x = std::clamp(transform.rot.x, -kRotXLimit, kRotXLimit);
	// Y軸回転が一定以上回ったらラップする
	while (transform.rot.y > DX_PI_F * 2) transform.rot.y -= DX_PI_F * 2;
	while (transform.rot.y < 0.0f) transform.rot.y += DX_PI_F * 2;

	// ターゲット位置を計算
	m_targetPos = m_pPlayer.lock()->GetComponent<Transform>()->pos;

	// ベクトルを回転させて位置を求める
	Vector3 rootVec = Vector3(0, 0, -kDefaultDist);

	Matrix4x4 rotYMat = Matrix4x4::GetRotY(transform.rot.y);
	Matrix4x4 rotXMat = Matrix4x4::GetRotX(transform.rot.x);
	Matrix4x4 transMat = Matrix4x4::GetTranslate(m_targetPos);

	// 求めた位置を代入して計算
	Matrix4x4 mat = rotXMat * rotYMat * transMat;
	transform.pos = mat * rootVec;

	// Y位置オフセットを適用
	m_targetPos.y += kPosOffset.y;
	transform.pos.y += kPosOffset.y;

	// X位置オフセットを適用
	Vector3 rightVec = Vector3::Up().Cross(m_targetPos - transform.pos).Normalized();
	m_targetPos += rightVec * kPosOffset.x;
	transform.pos += rightVec * kPosOffset.x;

	// エイム時に視野角を狭める
	if (m_pPlayer.lock()->IsAim())
	{
		m_fov = std::lerp(m_fov, kAimFov, 0.5f);
	}
	else
	{
		m_fov = std::lerp(m_fov, kDefaultFov, 0.5f);
	}
	SetupCamera_Perspective(m_fov);

	// DxLibのカメラに適用
	SetCameraPositionAndTarget_UpVecY(transform.pos, m_targetPos);
}

void Camera::Draw()
{
	// 処理なし
}

void Camera::Control()
{
	Transform& transform = *GetComponent<Transform>();
	Vector2 stick = PadInput::GetInstance().GetStickInput(PadInput::LR::Right);
	transform.rot.y += stick.x * kSensitivity;
	transform.rot.x += -stick.y * kSensitivity;
}
