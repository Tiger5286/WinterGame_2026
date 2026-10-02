#include "Player.h"
#include "Resource/ResourceManager.h"
#include "System/PadInput.h"
#include "Game/GameObjects/Camera/Camera.h"
#include "Utility/MyLib.h"
#include "State/PlayerStateIdle.h"
#include "Components/Animator/Animator.h"
#include "Components/Physics.h"
#include "Components/State/StateMachine.h"
#include "Components/Transform.h"
#include "Components/Collider/CapsuleCollider.h"
#include "State/PlayerStateMove.h"
#include "Resource/Model.h"
#include "System/ServiceLocator.h"
#include "Game/CollisionManager.h"
#include <cmath>

namespace
{
	constexpr float kColliderRadius = 30.0f;
	constexpr float kColliderHeight = 180.0f;

	// エイムしたときにカメラの向きに合わせるのにかかる時間
	constexpr int kAimStartFrame = 10;
}

Player::Player()
{
	AddComponent<Animator>();
	AddComponent<Physics>();
	AddComponent<StateMachine<Player>>(*this);
	AddComponent<CapsuleCollider>(*GetComponent<Transform>(), kColliderRadius, kColliderHeight);
}

Player::~Player()
{
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
			name == kAnimNames[static_cast<int>(Player::AnimationID::Land)])
			isLoop = false;
		animator->AddAnimation(name, 0.5f, isLoop);
	}

	// ステートを初期化
	auto stateMachine = GetComponent<StateMachine<Player>>();
	stateMachine->ChangeState(std::make_unique<PlayerStateIdle>(*this));
}

void Player::Update()
{
	Transform& transform = *GetComponent<Transform>();

	UpdateAim();

	// ステートを更新
	GetComponent<StateMachine<Player>>()->Update();
	// エイムしているときは向きをカメラの向きに固定する
	if (m_isAim)
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
	}
	else
	{
		m_aimStartFrame = 0;
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

#ifdef _DEBUG
	GetComponent<CapsuleCollider>()->Draw();
	Vector3 vel = GetComponent<Physics>()->m_vel;
	Vector3 acc = GetComponent<Physics>()->m_accel;
	DrawFormatString(100, 100, 0xff0000, L"Player:vel x:%.1f,y:%.1f,z:%.1f", vel.x, vel.y, vel.z);
	DrawFormatString(100, 100 + 16, 0xff0000, L"Player:acc x:%.1f,y:%.1f,z:%.1f", acc.x, acc.y, acc.z);
	DrawFormatString(100, 100 + 16 * 2, 0xff0000, L"Direction8:%d", static_cast<int>(PadInput::GetInstance().GetStickDirection8(PadInput::LR::Left)));
#endif
}

void Player::UpdateAnimation()
{
	PlayerState::ID stateID = GetComponent<StateMachine<Player>>()->GetState<PlayerState>()->GetID();
	auto animator = GetComponent<Animator>();
	auto physics = GetComponent<Physics>();
	// 上下別に再生可能なステートかどうか
	bool canSplitBody =
		stateID == PlayerState::ID::Idle ||
		stateID == PlayerState::ID::Move ||
		stateID == PlayerState::ID::Hover ||
		stateID == PlayerState::ID::Land;
	// 上下別に再生可能なステートかつエイム中のときだけ上下別アニメーションを再生する
	animator->SetSplitBody(canSplitBody && m_isAim);

	switch (stateID)
	{
	case PlayerState::ID::Idle:
		// 全身は通常待機
		animator->Play(kAnimNames[static_cast<int>(AnimationID::Idle)],Animator::Layer::FullBody);
		// 上下別アニメーションはエイム待機
		animator->Play(kAnimNames[static_cast<int>(AnimationID::AimIdle)], Animator::Layer::UpperBody);
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
		animator->Play(kAnimNames[static_cast<int>(animation)], Animator::Layer::LowerBody);
		animator->Play(kAnimNames[static_cast<int>(AnimationID::AimWalkForward)], Animator::Layer::UpperBody);
		break;
	}
	case PlayerState::ID::Jump:
		// ジャンプは全身アニメーションのみ
		animator->Play(kAnimNames[static_cast<int>(AnimationID::Jump)], Animator::Layer::FullBody);
		break;
	case PlayerState::ID::Fall:
		// 落下は全身アニメーションのみ
		animator->Play(kAnimNames[static_cast<int>(AnimationID::Fall)], Animator::Layer::FullBody);
		break;
	case PlayerState::ID::Land:
		// 着地は全身アニメーションのみ
		animator->Play(kAnimNames[static_cast<int>(AnimationID::Land)], Animator::Layer::FullBody);
		break;
	case PlayerState::ID::Hover:
		if (m_isAim)	// ホバリング中にエイムしている場合は上下別アニメーションを再生
		{
			animator->PlaySynced(kAnimNames[static_cast<int>(AnimationID::Hover)], Animator::Layer::FullBody, Animator::Layer::LowerBody);
			animator->Play(kAnimNames[static_cast<int>(AnimationID::AimIdle)], Animator::Layer::UpperBody);
			animator->Play(kAnimNames[static_cast<int>(AnimationID::Hover)], Animator::Layer::LowerBody);
		}
		else	// ホバリング中にエイムしていない場合は全身アニメーションを再生
		{
			animator->PlaySynced(kAnimNames[static_cast<int>(AnimationID::Hover)], Animator::Layer::FullBody, Animator::Layer::LowerBody);
			animator->PlaySynced(kAnimNames[static_cast<int>(AnimationID::Hover)], Animator::Layer::FullBody, Animator::Layer::UpperBody);
			animator->Play(kAnimNames[static_cast<int>(AnimationID::Hover)], Animator::Layer::FullBody);
		}
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
