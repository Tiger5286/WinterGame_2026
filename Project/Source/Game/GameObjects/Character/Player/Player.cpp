#include "Player.h"
#include "Resource/ResourceManager.h"
#include "System/PadInput.h"
#include "Game/GameObjects/Camera/Camera.h"
#include "Utility/MyLib.h"
#include "State/PlayerStateIdle.h"
#include "State/PlayerStateHit.h"
#include "Components/Animator/Animator.h"
#include "Components/Physics.h"
#include "Components/State/StateMachine.h"
#include "Components/Transform.h"
#include "Components/Collider/CapsuleCollider.h"
#include "State/PlayerStateMove.h"
#include "Resource/Model.h"
#include "System/ServiceLocator.h"
#include "Game/CollisionManager.h"
#include "Game/GameObjectManager.h"
#include "Game/HackingManager.h"
#include <cmath>
#include "Game.h"
#include "Components/Hackable.h"
#include "Game/UI/HackingTargetUI.h"
#include "System/UIManager.h"

namespace
{
	constexpr float kColliderRadius = 30.0f;
	constexpr float kColliderHeight = 180.0f;

	// エイムしたときにカメラの向きに合わせるのにかかる時間
	constexpr int kAimStartFrame = 10;

	constexpr const wchar_t* kHaveGunFrameName = L"mixamorig:RightHand";
	constexpr float kGunScale = 0.25f;

	constexpr int kShotInterval = 16;

	// ハッキング可能距離
	constexpr float kCanHackingDist = 1500.0f;

	constexpr int kMaxHp = 1000;
}

Player::Player()
{
	AddComponent<Animator>();
	AddComponent<Physics>();
	AddComponent<StateMachine<Player>>(*this);
	AddComponent<CapsuleCollider>(*GetComponent<Transform>(), kColliderRadius, kColliderHeight);
	m_tag = Tag::Player;
}

Player::~Player()
{
	m_pHackingTargetUI->Destroy();
}

void Player::Init()
{
	// CollisionManagerに登録
	ServiceLocator::GetInstance().GetCollisionManager().Register(shared_from_this());

	// physicsを初期化
	GetComponent<Physics>()->Init(GetComponent<Transform>(), GetComponent<CapsuleCollider>());

	// モデルを取得してアニメーションを初期化
	m_pModel = ResourceManager::GetInstance().DuplicateModel(L"PlayerModel");
	auto animator = GetComponent<Animator>();
	animator->Init(m_pModel.get());
	for (const auto& name : kAnimNames)	// アニメーションを追加
	{
		bool isLoop = true;
		if (name == kAnimNames[static_cast<int>(Player::AnimationID::Jump)] ||
			name == kAnimNames[static_cast<int>(Player::AnimationID::Land)] ||
			name == kAnimNames[static_cast<int>(Player::AnimationID::DodgeForward)] ||
			name == kAnimNames[static_cast<int>(Player::AnimationID::Hit)] ||
			name == kAnimNames[static_cast<int>(Player::AnimationID::Death)])
			isLoop = false;
		animator->AddAnimation(name, 0.5f, isLoop);
	}
	// 銃のモデルを取得して初期化
	m_pGunModel = ResourceManager::GetInstance().DuplicateModel(L"GunModel");

	m_haveGunFrameIndex = MV1SearchFrame(m_pModel->GetHandle(), kHaveGunFrameName);
	assert(m_haveGunFrameIndex != -1);

	// ステートを初期化
	auto stateMachine = GetComponent<StateMachine<Player>>();
	stateMachine->ChangeState(std::make_unique<PlayerStateIdle>(*this));

	m_pHackingTargetUI = std::make_shared<HackingTargetUI>();
	UIManager::GetInstance().AddUI(m_pHackingTargetUI);

	// HPを初期化
	m_hp = kMaxHp;
}

void Player::Update()
{
	m_frame++;

	auto& input = PadInput::GetInstance();

	Transform& transform = *GetComponent<Transform>();

	UpdateAim();

	// ステートを更新
	GetComponent<StateMachine<Player>>()->Update();

	// 視界の中心に近い敵を保存
	Vector2 screenPos;
	auto target = FindNearestVisibleHackable(screenPos);

	// ハッキング対象を示すUIを更新
	if (target)
	{
		HackingTargetUI::Info info;
		info.screenPos = screenPos;
		m_pHackingTargetUI->SetInfo(info);
		m_pHackingTargetUI->SetIsVisible(true);
	}
	else
	{
		m_pHackingTargetUI->SetIsVisible(false);
	}

	if (m_shotIntervalFrame > 0)
	{
		m_shotIntervalFrame--;
	}

	// 射撃
	if (input.IsPressedTrigger(PadInput::LR::Right))
	{
		if (m_shotIntervalFrame == 0)
		{
			const Vector3 cameraPos = m_pCamera.lock()->GetComponent<Transform>()->pos;
			const Vector3& target = m_pCamera.lock()->GetTargetPos();
			Vector3 vec = target - cameraPos;
			vec.Normalize();
			vec *= 2000;
			Vector3 end = cameraPos + vec;

			CollisionManager::ShotInfo shotInfo;
			shotInfo.damage = 50;
			ServiceLocator::GetInstance().GetCollisionManager().CheckCollShot(cameraPos, end, shotInfo);

			m_shotIntervalFrame = kShotInterval;
			m_isShooting = true;
		}
	}
	else
	{
		m_isShooting = false;
	}

	// エイムまたは射撃しているときは向きをカメラの向きに固定する
	if (m_isAim || m_isShooting)
	{
		m_angle = m_pCamera.lock()->GetComponent<Transform>()->rot.y - DX_PI_F;
		m_isRun = false;

		m_aimStartFrame++;
		if (m_aimStartFrame > kAimStartFrame)
		{
			m_aimStartAngle = m_angle;
		}
		float diff = MyLib::GetAngleDiff(m_angle, m_aimStartAngle);
		float rate = static_cast<float>(m_aimStartFrame) / static_cast<float>(kAimStartFrame);
		transform.rot.y = m_aimStartAngle + diff * rate;

		// モデルの向きを更新
		float cameraRotX = m_pCamera.lock()->GetComponent<Transform>()->rot.x;
		GetComponent<Animator>()->SetAimAngle(-cameraRotX);

		// ハッキング対象があればハッキングマネージャーに渡す
		m_pHackingManager.lock()->SetTarget(target);
	}
	else
	{
		m_aimStartFrame = 0;
		GetComponent<Animator>()->SetAimAngle(0.0f);
		m_pHackingManager.lock()->SetTarget(nullptr);
	}

	// physicsの更新
	GetComponent<Physics>()->Update();

	// 奈落に落ちたら原点に戻す
	if (transform.pos.y < -2000.0f)
	{
		transform.pos = Vector3::Zero();
	}

	// 向きを更新
	float diff = MyLib::GetAngleDiff(m_angle, transform.rot.y);
	transform.rot.y += diff * 0.1f;

	// モデルの行列を更新
	m_pModel->SetTransform(transform);

	// アニメーションを更新
	UpdateAnimation();
}

void Player::Draw()
{
	// モデルを描画
	m_pModel->Draw();
	GetComponent<StateMachine<Player>>()->Draw();	// ステートに描画したい内容があったら描画

	// エイムしているときのみ銃を描画
	if (m_isAim || m_isShooting)
	{
		MATRIX handWorld = MV1GetFrameLocalWorldMatrix(m_pModel->GetHandle(), m_haveGunFrameIndex);

		const MATRIX gunScale = MGetScale(VGet(kGunScale, kGunScale, kGunScale));
		const MATRIX gunRotX = MGetRotX(DX_PI_F / 2);
		const MATRIX gunRotY = MGetRotY(DX_PI_F / 2);
		const MATRIX gunRotZ = MGetRotZ(DX_PI_F / 180 * 10);
		const MATRIX gunTrans = MGetTranslate(Vector3(0, 10, 0));

		MATRIX gunLocal = gunScale;
		gunLocal = MMult(gunLocal, gunRotX);
		gunLocal = MMult(gunLocal, gunRotY);
		gunLocal = MMult(gunLocal, gunRotZ);
		gunLocal = MMult(gunLocal, gunTrans);
		const MATRIX gunWorld = MMult(gunLocal, handWorld);

		MV1SetMatrix(m_pGunModel->GetHandle(), gunWorld);
		m_pGunModel->Draw();

#ifdef _DEBUG
		// 銃口から線を描画
		const Vector3 cameraPos = m_pCamera.lock()->GetComponent<Transform>()->pos;
		const Vector3& target = m_pCamera.lock()->GetTargetPos();
		Vector3 vec = target - cameraPos;
		vec.Normalize();
		vec *= 2000;
		Vector3 end = cameraPos + vec;
		auto result = ServiceLocator::GetInstance().GetCollisionManager().RayCast(cameraPos, end);
		if (result.isHit)
		{
			const Vector3 muzzleLocal(0.0f, 35.0f, -224.0f);
			const Vector3 muzzleWorld = Vector3::FromDxLib(VTransform(muzzleLocal, gunWorld));
			DrawLine3D(muzzleWorld, result.hitPos, 0xff0000);
		}
#endif
	}

#ifdef _DEBUG
	GetComponent<CapsuleCollider>()->Draw();
	Vector3 vel = GetComponent<Physics>()->m_vel;
	Vector3 acc = GetComponent<Physics>()->m_accel;
	DrawFormatString(100, 100, 0xff0000, L"Player:vel x:%.1f,y:%.1f,z:%.1f", vel.x, vel.y, vel.z);
	DrawFormatString(100, 100 + 16, 0xff0000, L"Player:acc x:%.1f,y:%.1f,z:%.1f", acc.x, acc.y, acc.z);
	DrawFormatString(100, 100 + 16 * 2, 0xff0000, L"Direction8:%d", static_cast<int>(PadInput::GetInstance().GetStickDirection8(PadInput::LR::Left)));
#endif
}

void Player::OnHitAttack(const CollisionManager::AttackInfo& info)
{
	// HPが0なら処理しない
	if (m_hp <= 0) return;

	auto stateMachine = GetComponent<StateMachine<Player>>();

	// 被弾中は処理しない
	if (stateMachine->GetState<PlayerStateHit>()) return;

	m_hp -= info.damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		printfDx(L"死亡");
	}
	else
	{
		printfDx(L"被弾");
		stateMachine->ChangeState(std::make_unique<PlayerStateHit>(*this));
	}
}

void Player::UpdateAnimation()
{
	auto state = GetComponent<StateMachine<Player>>()->GetState<PlayerState>();
	PlayerState::ID stateID = GetComponent<StateMachine<Player>>()->GetState<PlayerState>()->GetID();
	auto animator = GetComponent<Animator>();
	auto physics = GetComponent<Physics>();
	// 上下別に再生可能なステートかつエイム中のときだけ上下別アニメーションを再生する
	const bool canSplitBody = (m_isAim || m_isShooting) && state->CanSplitBody();
	animator->SetSplitBody(canSplitBody);

	std::function playShotAnim = [this, animator]() {
		if (m_isAim)
		{
			if (m_isShooting)
			{
				animator->Play(kAnimNames[static_cast<int>(AnimationID::AimFire)], Animator::Layer::UpperBody);
			}
			else
			{
				animator->Play(kAnimNames[static_cast<int>(AnimationID::AimIdle)], Animator::Layer::UpperBody);
			}
		}
		else if (m_isShooting)
		{
			animator->Play(kAnimNames[static_cast<int>(AnimationID::AimFire)], Animator::Layer::UpperBody);
		}
		};

	switch (stateID)
	{
	case PlayerState::ID::Idle:
		// 全身は通常待機
		animator->Play(kAnimNames[static_cast<int>(AnimationID::Idle)], Animator::Layer::FullBody);
		// 上下別アニメーションはエイム待機
		playShotAnim();
		animator->Play(kAnimNames[static_cast<int>(AnimationID::AimIdle)], Animator::Layer::LowerBody);
		break;
	case PlayerState::ID::Move:
	{
		// 全身は走るか歩くかで切り替え
		if (physics->GetSquaredMoveSpeed() > PlayerStateMove::kMaxJogSpeed * PlayerStateMove::kMaxJogSpeed)
		{
			animator->Play(kAnimNames[static_cast<int>(AnimationID::Run)], Animator::Layer::FullBody);
		}
		else
		{
			animator->Play(kAnimNames[static_cast<int>(AnimationID::Jog)], Animator::Layer::FullBody);
		}
		// 上下別アニメーションはエイム中のときだけ切り替え
		const auto direction = PadInput::GetInstance().GetStickDirection8(PadInput::LR::Left);
		AnimationID animation = AnimationID::AimIdle;
		switch (direction)
		{
		case PadInput::Direction8::Right:
			animation = AnimationID::AimWalkRight;
			break;
		case PadInput::Direction8::UpRight:
			animation = AnimationID::AimWalkForwardRight;
			break;
		case PadInput::Direction8::Up:
			animation = AnimationID::AimWalkForward;
			break;
		case PadInput::Direction8::UpLeft:
			animation = AnimationID::AimWalkForwardLeft;
			break;
		case PadInput::Direction8::Left:
			animation = AnimationID::AimWalkLeft;
			break;
		case PadInput::Direction8::DownLeft:
			animation = AnimationID::AimWalkBackwardLeft;
			break;
		case PadInput::Direction8::Down:
			animation = AnimationID::AimWalkBackward;
			break;
		case PadInput::Direction8::DownRight:
			animation = AnimationID::AimWalkBackwardRight;
			break;
		case PadInput::Direction8::None:
			break;
		}
		playShotAnim();
		animator->Play(kAnimNames[static_cast<int>(animation)], Animator::Layer::LowerBody);
		break;
	}
	case PlayerState::ID::Jump:
		animator->Play(kAnimNames[static_cast<int>(AnimationID::Jump)], Animator::Layer::FullBody);
		playShotAnim();
		animator->Play(kAnimNames[static_cast<int>(AnimationID::Jump)], Animator::Layer::LowerBody);
		break;
	case PlayerState::ID::Fall:
		animator->Play(kAnimNames[static_cast<int>(AnimationID::Fall)], Animator::Layer::FullBody);
		playShotAnim();
		animator->Play(kAnimNames[static_cast<int>(AnimationID::Fall)], Animator::Layer::LowerBody);
		break;
	case PlayerState::ID::Land:
		animator->Play(kAnimNames[static_cast<int>(AnimationID::Land)], Animator::Layer::FullBody);
		playShotAnim();
		animator->Play(kAnimNames[static_cast<int>(AnimationID::Land)], Animator::Layer::LowerBody);
		break;
	case PlayerState::ID::Hover:
		animator->Play(kAnimNames[static_cast<int>(AnimationID::Hover)], Animator::Layer::FullBody);
		playShotAnim();
		animator->Play(kAnimNames[static_cast<int>(AnimationID::Hover)], Animator::Layer::LowerBody);
		break;
	case PlayerState::ID::Dodge:
		// 全身
		animator->Play(kAnimNames[static_cast<int>(AnimationID::DodgeForward)],Animator::Layer::FullBody);
		// 上下別
		playShotAnim();
		animator->Play(kAnimNames[static_cast<int>(AnimationID::DodgeForward)], Animator::Layer::LowerBody);
		break;
	case PlayerState::ID::Hit:
		animator->Play(kAnimNames[static_cast<int>(AnimationID::Hit)], Animator::Layer::FullBody);
		break;
	}
	animator->Update();
}

void Player::UpdateAim()
{
	auto& input = PadInput::GetInstance();
	if (input.IsTriggeredTrigger(PadInput::LR::Left))
	{
		m_aimStartAngle = m_angle;
	}
	if (input.IsPressedTrigger(PadInput::LR::Left))
	{
		m_isAim = true;
	}
	else
	{
		m_isAim = false;
	}
}

std::shared_ptr<GameObject> Player::FindNearestVisibleHackable(Vector2& screenPos)
{
	auto enemies = m_pGameObjectManager.lock()->GetHackables();

	if (enemies.empty()) return nullptr;

	std::shared_ptr<GameObject> nearestEnemy = nullptr;

	float nearestDistSq = FLT_MAX;

	Vector2 centerPos = Vector2(Game::kScreenWidth / 2, Game::kScreenHeight / 2);

	for (auto& weakEnemy : enemies)
	{
		auto enemy = weakEnemy.lock();
		// nullptrなら次へ
		if (!enemy) continue;
		// ハッキングできない相手は除外
		if (!enemy->GetComponent<Hackable>()) continue;
		// 敵の中心位置を計算
		Vector3 targetPos = enemy->GetComponent<Transform>()->pos;
		targetPos += enemy->GetComponent<Hackable>()->GetHackLocalPos();
		// 視界の外にいる敵を除外
		if (CheckCameraViewClip(targetPos)) continue;

		// 一定距離より遠い敵は除外
		float distSq = (GetComponent<Transform>()->pos - targetPos).SquaredLength();
		if (distSq > kCanHackingDist * kCanHackingDist) continue;

		// ワールド座標をスクリーン座標に変換する
		Vector3 screenPos3d = Vector3::FromDxLib(ConvWorldPosToScreenPos(targetPos));
		Vector2 screenPos2d = { screenPos3d.x,screenPos3d.y };
		// 画面中央からの距離の2乗
		float screenDistSq = (screenPos2d - centerPos).SquaredLength();
		if (screenDistSq < nearestDistSq)
		{
			nearestDistSq = screenDistSq;
			nearestEnemy = enemy;
			screenPos = screenPos2d;
		}
	}

	return nearestEnemy;
}
