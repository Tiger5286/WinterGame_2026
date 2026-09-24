#include "Camera.h"
#include "Game/GameObjects/Player/Player.h"
#include "Utility/Matrix4x4.h"
#include "System/PadInput.h"
#include <algorithm>

namespace
{
	constexpr float kDefaultDist = 150.0f;
	const Vector3 kPosOffset = Vector3(70.0f, 150.0f,0.0f);

	constexpr float kSensitivity = DX_PI_F / 60;

	constexpr float kRotXLimit = DX_PI_F / 2 - 0.01f;
}

void Camera::Init()
{
	m_transform.pos = Vector3(0,0,0);
	SetCameraPositionAndTarget_UpVecY(m_transform.pos, m_targetPos);
}

void Camera::Update()
{
	// 入力を反映
	Control();
	// 回転の数値を調整
	// X軸回転の上限下限を設定
	m_transform.rot.x = std::clamp(m_transform.rot.x, -kRotXLimit, kRotXLimit);
	// Y軸回転が一定以上回ったらラップする
	while (m_transform.rot.y > DX_PI_F * 2) m_transform.rot.y -= DX_PI_F * 2;
	while (m_transform.rot.y < 0.0f) m_transform.rot.y += DX_PI_F * 2;

	// ターゲット位置を計算
	m_targetPos = m_pPlayer.lock()->GetTransform().pos;

	// ベクトルを回転させて位置を求める
	Vector3 rootVec = Vector3(0, 0, -kDefaultDist);

	Matrix4x4 rotYMat = Matrix4x4::GetRotY(m_transform.rot.y);
	Matrix4x4 rotXMat = Matrix4x4::GetRotX(m_transform.rot.x);
	Matrix4x4 transMat = Matrix4x4::GetTranslate(m_targetPos);

	// 求めた位置を代入して計算
	Matrix4x4 mat = rotXMat * rotYMat * transMat;
	m_transform.pos = mat * rootVec;

	// Y位置オフセットを適用
	m_targetPos.y += kPosOffset.y;
	m_transform.pos.y += kPosOffset.y;

	// X位置オフセットを適用
	Vector3 rightVec = Vector3::Up().Cross(m_targetPos - m_transform.pos).Normalized();
	m_targetPos += rightVec * kPosOffset.x;
	m_transform.pos += rightVec * kPosOffset.x;

	// DxLibのカメラに適用
	SetCameraPositionAndTarget_UpVecY(m_transform.pos, m_targetPos);
}

void Camera::Draw() const
{
	// 処理なし
}

void Camera::Control()
{
	Vector2 stick = PadInput::GetInstance().GetStickInput(PadInput::LR::Right);
	m_transform.rot.y += stick.x * kSensitivity;
	m_transform.rot.x += -stick.y * kSensitivity;
}
