#include "Camera.h"
#include "Game/GameObjects/Character/Player/Player.h"
#include "Utility/Matrix4x4.h"
#include "System/PadInput.h"
#include <algorithm>
#include "Components/Transform.h"
#include <cmath>
#include "Effekseer.h"
#include "System/ServiceLocator.h"
#include "Game/CollisionManager.h"

namespace
{
	constexpr float kAimFov = DX_PI_F / 180 * 30;

	constexpr float kDefaultDist = 250.0f;
	const Vector3 kPosOffset = Vector3(70.0f, 170.0f,0.0f);
	const Vector3 kAimPosOffset = Vector3(0.0f, 0.0f, 0.0f);

	constexpr float kSensitivity = DX_PI_F / 60;
	constexpr float kAimSensitivity = DX_PI_F / 120;

	constexpr float kRotXLimit = DX_PI_F / 2 - 0.01f;
	constexpr float kRotXLimitAim = DX_PI_F / 4;

	// 地形に当たったとき、壁からどれだけ手前にカメラを置くか
	constexpr float kWallMargin = 15.0f;
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
	if (m_pPlayer.lock()->IsAim())
	{
		transform.rot.x = std::clamp(transform.rot.x, -kRotXLimitAim, kRotXLimitAim);
	}
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

	// オフセットを設定
	Vector3 offset = kPosOffset;
	// エイムしていたら専用のオフセットを足す
	if (m_pPlayer.lock()->IsAim())
	{
		offset += kAimPosOffset;
	}

	// Y位置オフセットを適用
	m_targetPos.y += offset.y;
	transform.pos.y += offset.y;

	// X位置オフセットを適用
	Vector3 rightVec = Vector3::Up().Cross(m_targetPos - transform.pos).Normalized();
	m_targetPos += rightVec * offset.x;
	transform.pos += rightVec * offset.x;

	// カメラと地形の当たり判定
	// 敵などのカプセルには反応させず、地形(ポリゴン)とだけ判定する
	auto result = ServiceLocator::GetInstance().GetCollisionManager().RayCast(m_targetPos, transform.pos, true);
	if (result.isHit)
	{
		Vector3 toCamera = result.hitPos - m_targetPos;
		float dist = toCamera.Length();
		// 壁から少し手前に置く(注視点を通り越さないように最低距離を残す)
		float newDist = (std::max)(dist - kWallMargin, 1.0f);
		transform.pos = m_targetPos + toCamera.Normalized() * newDist;
	}

	// ライトの方向をカメラの向きに合わせる
	Vector3 lightDir = (m_targetPos - transform.pos).Normalized();
	// y軸の方向を調整する
	lightDir.y = -0.5f;
	SetLightDirection(lightDir);

	// エイム時に視野角を狭める
	if (m_pPlayer.lock()->IsAim())
	{
		m_fov = std::lerp(m_fov, kAimFov, 0.15f);
	}
	else
	{
		m_fov = std::lerp(m_fov, kDefaultFov, 0.05f);
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
	float sensitivity = kSensitivity;
	if (m_pPlayer.lock()->IsAim())
	{
		sensitivity = kAimSensitivity;
	}
	transform.rot.y += stick.x * sensitivity;
	transform.rot.x += -stick.y * sensitivity;
}