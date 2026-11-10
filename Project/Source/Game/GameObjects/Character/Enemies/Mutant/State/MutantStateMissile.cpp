#include "MutantStateMissile.h"
#include "../Mutant.h"
#include "Components/State/StateMachine.h"
#include "MutantStateOrbit.h"
#include "Game/GameObjects/Missile.h"
#include "Game/GameObjects/Character/Player/Player.h"
#include "Components/Transform.h"
#include "Game/GameObjectManager.h"
#include "Components/Animator/Animator.h"

namespace
{
	// 何フレームごとにミサイルを発射するか
	constexpr int kMissileShotInterval = 60;
	// ミサイルを何発発射するか
	constexpr int kMissileShotCount = 4;
}

MutantStateMissile::MutantStateMissile(Mutant& mutant) :
	MutantState(mutant)
{
}

MutantStateMissile::~MutantStateMissile()
{
}

void MutantStateMissile::Enter()
{
	m_owner.GetComponent<Animator>()->Play(L"Mutant|Power");
}

void MutantStateMissile::Update()
{
	m_frameCount++;

	if (m_frameCount % kMissileShotInterval == 0)
	{
		// GameObjectManagerのnullチェック
		if (GetGameObjectManager())
		{
			// ミサイルの位置を設定
			Vector3 pos;
			pos = GetPlayer()->GetComponent<Transform>()->pos;	// プレイヤーの位置にミサイルを生成
			// ボスの高さは床面に固定、かつ地面は平らなので、ボスの高さ=床の高さ
			pos.y = m_owner.GetComponent<Transform>()->pos.y + 1.0f;	// 描画が床にめり込まないように少し上げる
			// ミサイルを生成してGameObjectManagerに追加
			auto missile = std::make_shared<Missile>(pos);
			GetGameObjectManager()->Add(missile);
		}
		else
		{
			assert(false && "MutantStateMissile::Update() : GameObjectManagerが取得できませんでした");
		}
	}

	// 指定回数ミサイルを発射したらステート遷移
	if (m_frameCount >= kMissileShotInterval * kMissileShotCount)
	{
		m_owner.GetComponent<StateMachine<Mutant>>()->ChangeState(std::make_unique<MutantStateOrbit>(m_owner));
		return;
	}
}

void MutantStateMissile::Exit()
{
}
