#include "Drone.h"
#include "Components/Collider/CapsuleCollider.h"
#include "Components/Hitbox/Hitbox.h"
#include "Components/Hitbox/HitboxSphere.h"
#include "Components/Hackable.h"
#include "Components/State/StateMachine.h"
#include "Components/Transform.h"
#include "Components/Animator/SimpleAnimator.h"
#include "System/ServiceLocator.h"
#include "System/UIManager.h"
#include "Game/CollisionManager.h"
#include "Game/UI/EnemyHPUI.h"
#include "Game/GameObjects/Character/Player/Player.h"
#include "Resource/ResourceManager.h"
#include "Resource/Model.h"
#include "Utility/MyLib.h"
#include "State/DroneStateIdle.h"
#include "State/DroneStateFall.h"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace
{
	constexpr float kColliderRadius = 35.0f;
	constexpr float kColliderHeight = 70.0f;

	// モデルに入っているアニメーションの名前
	constexpr const wchar_t* kAnimName = L"Scene";

	// 銃のフレームの名前と、そのフレームから見た銃口の位置
	// (2本の銃身の真ん中の先端。モデル制作時の数値からの目安なので、ずれていたら調整する)
	constexpr const wchar_t* kTurretFrameName = L"Drone_Turret";
	const Vector3 kMuzzleLocalPos = { 0.0f,-7.0f,-39.0f };
	// モデルの前方向(フレームから見た銃の向き)
	const Vector3 kModelFront = { 0.0f,0.0f,-1.0f };

	// 銃を上下に向けられる角度の制限
	const float kMaxTurretPitchUp = MyLib::DegreeToRadian(30.0f);
	const float kMaxTurretPitchDown = MyLib::DegreeToRadian(60.0f);

	// プレイヤーを見つけられる距離
	constexpr float kSearchRange = 2000.0f;
	// レーザーの最大の長さ
	constexpr float kLaserLength = 3000.0f;
	// 射撃のダメージ
	constexpr int kShotDamage = 50;

	const Vector3 kHackLocalPos = { 0.0f,Drone::kBodyCenterHeight,0.0f };
	constexpr const wchar_t* kHackBoardPath = L"data/hackBoard/droneBoard.csv";

	const Vector3 kHPUIOffset = { 0.0f,120.0f,0.0f };

	// 目のマテリアルの名前と、ハック中の目の色(Jetと同じシアン)
	constexpr const wchar_t* kEyeMaterialName = L"Eye";
	const COLOR_F kHackedEyeColor = { 0.25f,0.75f,1.0f,1.0f };
	// チラついている間の目の色(消えたように暗くする)
	const COLOR_F kGlitchEyeColor = { 0.0f,0.0f,0.0f,1.0f };
	// チラついている間の本体の色の倍率(白っぽく飛ばす)
	const COLOR_F kGlitchBodyColorScale = { 2.0f,2.0f,2.0f,1.0f };
	const COLOR_F kNormalBodyColorScale = { 1.0f,1.0f,1.0f,1.0f };

	// チラつきの間隔(この範囲でランダム)と、チラつく長さ
	constexpr int kGlitchWaitMin = 20;
	constexpr int kGlitchWaitMax = 60;
	constexpr int kGlitchFrame = 3;
	// 揺れの大きさ(いつも / チラついている時)
	constexpr float kShakeAmount = 1.0f;
	constexpr float kGlitchShakeAmount = 4.0f;

	// -amount～amountのランダムな値を返す
	float GetRandRange(float amount)
	{
		return (static_cast<float>(GetRand(1000)) / 1000.0f * 2.0f - 1.0f) * amount;
	}
}

Drone::Drone(std::shared_ptr<Player> pPlayer) :
	EnemyBase(pPlayer)
{
	// 出現位置から動かないのでPhysicsは付けない
	AddComponent<CapsuleCollider>(*GetComponent<Transform>(), kColliderRadius, kColliderHeight);
	AddComponent<Hitbox>();
	AddComponent<Hackable>();
	AddComponent<StateMachine<Drone>>(*this);
	AddComponent<SimpleAnimator>();
	m_tag = Tag::Enemy;
}

Drone::~Drone()
{
}

void Drone::Init()
{
	ServiceLocator::GetInstance().GetCollisionManager().Register(shared_from_this());

	m_pModel = ResourceManager::GetInstance().DuplicateModel(L"Drone");
	m_turretFrameIndex = MV1SearchFrame(m_pModel->GetHandle(), kTurretFrameName);
	assert(m_turretFrameIndex >= 0 && "Drone::Init() : 銃のフレームが見つかりませんでした");
	// 銃を回すときの基準にするため、元のローカル行列を覚えておく
	m_turretDefaultMatrix = MV1GetFrameLocalMatrix(m_pModel->GetHandle(), m_turretFrameIndex);

	// 目のマテリアルを探して、ハックされていない時に戻すため元の色を覚えておく
	const int materialNum = MV1GetMaterialNum(m_pModel->GetHandle());
	for (int i = 0; i < materialNum; i++)
	{
		if (std::wstring(MV1GetMaterialName(m_pModel->GetHandle(), i)) == kEyeMaterialName)
		{
			m_eyeMaterialIndex = i;
			break;
		}
	}
	assert(m_eyeMaterialIndex >= 0 && "Drone::Init() : 目のマテリアルが見つかりませんでした");
	if (m_eyeMaterialIndex >= 0)
	{
		m_eyeDefaultDifColor = MV1GetMaterialDifColor(m_pModel->GetHandle(), m_eyeMaterialIndex);
		m_eyeDefaultEmiColor = MV1GetMaterialEmiColor(m_pModel->GetHandle(), m_eyeMaterialIndex);
	}

	// 銃口の位置を最初から正しく取れるように、先にモデルの位置を合わせておく
	ApplyModelTransform();

	auto animator = GetComponent<SimpleAnimator>();
	animator->Init(m_pModel.get());
	animator->Play(kAnimName);

	auto stateMachine = GetComponent<StateMachine<Drone>>();
	stateMachine->ChangeState(std::make_unique<DroneStateIdle>(*this));

	Hackable::Info hackableInfo;
	hackableInfo.func = [this](Hackable::HackedData data) { OnHacked(data); };
	hackableInfo.hackLocalPos = kHackLocalPos;
	GetComponent<Hackable>()->SetInfo(hackableInfo);
	GetComponent<Hackable>()->LoadData(kHackBoardPath);
	GetComponent<Hackable>()->Init();

	SetHitbox();

	m_hp = kMaxHp;
	m_pHPUI = std::make_shared<EnemyHPUI>();
	UIManager::GetInstance().AddUI(m_pHPUI);
}

void Drone::Update()
{
	GetComponent<StateMachine<Drone>>()->Update();

	auto hackable = GetComponent<Hackable>();
	hackable->Update();
	UpdateHackedLook(hackable->IsHacked());

	ApplyModelTransform();
	ApplyTurretPitch();

	GetComponent<SimpleAnimator>()->Update(GetTimeScale());

	EnemyHPUI::Info info;
	info.uiPos = GetComponent<Transform>()->pos + kHPUIOffset;
	info.nowHP = m_hp;
	info.maxHP = kMaxHp;
	m_pHPUI->SetInfo(info);

	GetComponent<Hitbox>()->Update();
}

void Drone::Draw()
{
	m_pModel->Draw();
	GetComponent<StateMachine<Drone>>()->Draw();
#ifdef _DEBUG
	GetComponent<CapsuleCollider>()->Draw();
	GetComponent<Hitbox>()->Draw();
#endif
}

void Drone::OnWasShot(const CollisionManager::ShotInfo& info)
{
	// 落下中は撃たれても何もしない(落下ステートに入り直さない)
	if (IsFalling()) return;

	if (GetComponent<Hackable>()->IsHacked())
	{
		if (info.isWeakPoint)
		{
			m_hp -= info.damage * 1.5f;
		}
		else
		{
			m_hp -= info.damage;
		}
	}
	else
	{
		m_hp -= info.damage / 10;
	}

	if (m_hp <= 0)
	{
		m_hp = 0;
		GetComponent<StateMachine<Drone>>()->ChangeState(std::make_unique<DroneStateFall>(*this));
	}
}

bool Drone::IsFalling()
{
	return GetComponent<StateMachine<Drone>>()->GetState<DroneStateFall>() != nullptr;
}

void Drone::OnHacked(Hackable::HackedData data)
{
	// 見た目はUpdateHackedLook()で毎フレーム変えている
	// プレイヤーの弾を回復させる
	m_pPlayer.lock()->SetAmmoMax();
}

void Drone::SetHitbox()
{
	auto hitbox = GetComponent<Hitbox>();
	// 胴体
	HitboxSphere::Info sphere;
	sphere.frameName = L"Drone_Body";
	sphere.radius = 25.0f;
	sphere.pModel = m_pModel.get();
	sphere.isWeakPoint = false;
	hitbox->AddCollider(sphere);
	// 左右のプロペラ
	sphere.frameName = L"Drone_FanL";
	sphere.radius = 15.0f;
	hitbox->AddCollider(sphere);
	sphere.frameName = L"Drone_FanR";
	hitbox->AddCollider(sphere);
	// 銃
	sphere.frameName = kTurretFrameName;
	sphere.radius = 12.0f;
	sphere.localOffset = Vector3(0.0f, -7.0f, -20.0f);
	hitbox->AddCollider(sphere);
	// 目(弱点)
	sphere.radius = 6.0f;
	sphere.localOffset = Vector3(0.0f, -2.0f, -18.0f);
	sphere.isWeakPoint = true;
	hitbox->AddCollider(sphere);
}

void Drone::ApplyModelTransform()
{
	// モデルの原点は本体の中心なので、足元から中心の高さだけ上げて描画する
	Transform modelTransform = *GetComponent<Transform>();
	modelTransform.pos += Vector3::Up() * kBodyCenterHeight;
	// ハック中の揺れは描画位置だけに足す(Transformや当たり判定は動かさない)
	modelTransform.pos += m_shakeOffset;
	m_pModel->SetTransform(modelTransform);
}

void Drone::UpdateHackedLook(bool isHacked)
{
	const int handle = m_pModel->GetHandle();

	if (!isHacked)
	{
		// 元の見た目に戻す
		if (m_eyeMaterialIndex >= 0)
		{
			MV1SetMaterialDifColor(handle, m_eyeMaterialIndex, m_eyeDefaultDifColor);
			MV1SetMaterialEmiColor(handle, m_eyeMaterialIndex, m_eyeDefaultEmiColor);
		}
		MV1SetDifColorScale(handle, kNormalBodyColorScale);
		m_shakeOffset = Vector3::Zero();
		m_glitchWaitFrame = 0;
		m_glitchFrame = 0;
		return;
	}

	// チラつきのタイミングを数える
	if (m_glitchFrame > 0)
	{
		m_glitchFrame--;
	}
	else
	{
		m_glitchWaitFrame--;
		if (m_glitchWaitFrame <= 0)
		{
			// チラつきを始めて、次にチラつくまでの時間をランダムに決める
			m_glitchFrame = kGlitchFrame;
			m_glitchWaitFrame = kGlitchWaitMin + GetRand(kGlitchWaitMax - kGlitchWaitMin);
		}
	}
	const bool isGlitch = m_glitchFrame > 0;

	// 目の色：ふだんはシアン、チラついている間は消える
	if (m_eyeMaterialIndex >= 0)
	{
		const COLOR_F eyeColor = isGlitch ? kGlitchEyeColor : kHackedEyeColor;
		MV1SetMaterialDifColor(handle, m_eyeMaterialIndex, eyeColor);
		MV1SetMaterialEmiColor(handle, m_eyeMaterialIndex, eyeColor);
	}
	// 本体の色：チラついている間だけ白っぽく飛ばす
	MV1SetDifColorScale(handle, isGlitch ? kGlitchBodyColorScale : kNormalBodyColorScale);

	// 揺れ：いつも細かく震えていて、チラつく時は大きく揺れる
	const float shake = isGlitch ? kGlitchShakeAmount : kShakeAmount;
	m_shakeOffset = Vector3(GetRandRange(shake), GetRandRange(shake), GetRandRange(shake));
}

void Drone::ApplyTurretPitch()
{
	// 元のローカル行列の前にX軸回転をかけて、銃だけを上下に回す
	// (モデルの前方向は-Zなので、+の回転で銃口が上を向く)
	const MATRIX rotMat = MGetRotX(m_turretPitch);
	MV1SetFrameUserLocalMatrix(m_pModel->GetHandle(), m_turretFrameIndex, MMult(rotMat, m_turretDefaultMatrix));
}

float Drone::GetTargetYaw()
{
	const Vector3 toPlayer = GetPlayerCenterPos() - GetComponent<Transform>()->pos;
	return toPlayer.Angle();
}

float Drone::GetTargetPitch() const
{
	// 銃の回転の中心から見たプレイヤーの方向で角度を求める
	const MATRIX turretMat = MV1GetFrameLocalWorldMatrix(m_pModel->GetHandle(), m_turretFrameIndex);
	const Vector3 pivot = Vector3::FromDxLib(VTransform(Vector3::Zero(), turretMat));
	const Vector3 toPlayer = GetPlayerCenterPos() - pivot;
	const float horizontal = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z);
	const float pitch = std::atan2(toPlayer.y, horizontal);
	return std::clamp(pitch, -kMaxTurretPitchDown, kMaxTurretPitchUp);
}

void Drone::TurnToPlayerLerp(float rate)
{
	auto transform = GetComponent<Transform>();
	// 左右は角度の差を-π～πにしてから近づける(遠回りしないように)
	const float yawDiff = MyLib::GetAngleDiff(GetTargetYaw(), transform->rot.y);
	transform->rot.y += yawDiff * rate;
	m_turretPitch += (GetTargetPitch() - m_turretPitch) * rate;

	// すぐ銃口の位置と向きに反映させる
	ApplyModelTransform();
	ApplyTurretPitch();
}

void Drone::TurnToPlayerLimited(float maxAngle)
{
	auto transform = GetComponent<Transform>();
	const float yawDiff = MyLib::GetAngleDiff(GetTargetYaw(), transform->rot.y);
	transform->rot.y += std::clamp(yawDiff, -maxAngle, maxAngle);
	const float pitchDiff = GetTargetPitch() - m_turretPitch;
	m_turretPitch += std::clamp(pitchDiff, -maxAngle, maxAngle);

	// すぐ銃口の位置と向きに反映させる
	ApplyModelTransform();
	ApplyTurretPitch();
}

bool Drone::CanSeePlayer() const
{
	const Vector3 muzzlePos = GetMuzzlePos();
	const Vector3 playerPos = GetPlayerCenterPos();

	// 射程外なら見えない
	if ((playerPos - muzzlePos).SquaredLength() > kSearchRange * kSearchRange) return false;

	// 間に壁があれば見えない
	return !ServiceLocator::GetInstance().GetCollisionManager().RayCast(muzzlePos, playerPos, true).isHit;
}

Vector3 Drone::GetMuzzlePos() const
{
	// 銃のフレームのワールド行列で、フレームから見た銃口の位置を変換する
	const MATRIX turretMat = MV1GetFrameLocalWorldMatrix(m_pModel->GetHandle(), m_turretFrameIndex);
	return Vector3::FromDxLib(VTransform(kMuzzleLocalPos, turretMat));
}

Vector3 Drone::GetMuzzleDir() const
{
	// 銃のフレームの回転で、モデルの前方向を変換する(位置の移動は無視する)
	const MATRIX turretMat = MV1GetFrameLocalWorldMatrix(m_pModel->GetHandle(), m_turretFrameIndex);
	return Vector3::FromDxLib(VTransformSR(kModelFront, turretMat)).Normalized();
}

Vector3 Drone::GetPlayerCenterPos() const
{
	auto pPlayer = m_pPlayer.lock();
	auto capsule = pPlayer->GetComponent<CapsuleCollider>();
	// カプセルの下の球と上の球の真ん中を体の真ん中とする
	return (capsule->GetBottom() + capsule->GetTop()) * 0.5f;
}

Vector3 Drone::GetLaserEnd(const Vector3& start, const Vector3& dir, bool* isHitPlayer) const
{
	if (isHitPlayer) *isHitPlayer = false;

	Vector3 end = start + dir * kLaserLength;
	// 壁に当たったら、そこでレーザーを止める
	auto wallResult = ServiceLocator::GetInstance().GetCollisionManager().RayCast(start, end, true);
	if (wallResult.isHit) end = wallResult.hitPos;

	// 壁までの線分がプレイヤーに当たっていたら、プレイヤーのところで止める(貫通させない)
	auto capsule = m_pPlayer.lock()->GetComponent<CapsuleCollider>();
	auto playerResult = MyLib::CheckHitLineCapsule(capsule->GetBottom(), capsule->GetTop(), capsule->GetRadius(), start, end);
	if (playerResult.isHit)
	{
		if (isHitPlayer) *isHitPlayer = true;
		return playerResult.hitPos;
	}
	return end;
}

void Drone::Fire(const Vector3& start, const Vector3& dir)
{
	// レーザーがプレイヤーで止まったら当たり
	bool isHitPlayer = false;
	GetLaserEnd(start, dir, &isHitPlayer);
	if (isHitPlayer)
	{
		CollisionManager::AttackInfo info = {
			.damage = kShotDamage
		};
		m_pPlayer.lock()->OnHitAttack(info);
	}
}
