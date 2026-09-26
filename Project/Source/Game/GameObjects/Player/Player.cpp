#include "Player.h"
#include "Resource/ResourceManager.h"
#include "System/PadInput.h"
#include "Game/GameObjects/Camera/Camera.h"
#include "Utility/MyLib.h"
#include "State/PlayerStateIdle.h"
#include "Components/Animator/Animator.h"
#include "Components/Physics.h"
#include "Components/State/StateMachine.h"
#include "State/PlayerStateMove.h"

Player::Player()
{
	AddComponent<Animator>();
	AddComponent<Physics>();
	AddComponent<StateMachine<Player>>(*this);
}

void Player::Init()
{
	// physicsを初期化
	GetComponent<Physics>()->Init(GetComponent<Transform>(), Physics::kDefaultDrag, 0.0f);

	// モデルを取得してアニメーションを初期化
	m_pModel = ResourceManager::GetInstance().DuplicateModel(L"PlayerModel");
	auto animator = GetComponent<Animator>();
	animator->Init(m_pModel.get());
	for (const auto& name : kAnimNames)	// アニメーションを追加
	{
		animator->AddAnimation(name);
	}

	// ステートを初期化
	auto stateMachine = GetComponent<StateMachine<Player>>();
	stateMachine->ChangeState(std::make_unique<PlayerStateIdle>(*this));
}

void Player::Update()
{
	Transform& transform = *GetComponent<Transform>();

	// ステートを更新
	GetComponent<StateMachine<Player>>()->Update();

	// physicsの更新
	GetComponent<Physics>()->Update();

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
}

void Player::UpdateAnimation()
{
	PlayerState::ID stateID = GetComponent<StateMachine<Player>>()->GetState<PlayerState>()->GetID();
	auto animator = GetComponent<Animator>();
	auto physics = GetComponent<Physics>();

	switch (stateID)
	{
	case PlayerState::ID::Idle:
		animator->Play(kAnimNames[static_cast<int>(AnimationID::Idle)]);
		break;
	case PlayerState::ID::Move:
		if (physics->GetSquaredMoveSpeed() > PlayerStateMove::kMaxJogSpeed * PlayerStateMove::kMaxJogSpeed)
		{
			animator->Play(kAnimNames[static_cast<int>(AnimationID::Run)]);
		}
		else
		{
			animator->Play(kAnimNames[static_cast<int>(AnimationID::Jog)]);
		}
		break;
	}

	animator->Update();
}