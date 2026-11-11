#include "MutantStateMoveEdge.h"
#include "Utility/Vector3.h"
#include "Game/GameObjects/Character/Player/Player.h"
#include "Components/Transform.h"
#include "Components/Physics.h"
#include "../Mutant.h"
#include "MutantStateCharge.h"
#include "Components/State/StateMachine.h"

namespace
{
	// ステージの中心位置
	const Vector3 kStageCenterPos = Vector3(6750, 100, 250);
	// ステージの中心から端の距離
	constexpr float kStageRadius = 1000.0f;

	// 端に移動する時間
	constexpr int kMoveEdgeFrame = 30;

	// 突進に移るまでの時間
	constexpr int kStateEndFrame = 60;

	constexpr float kJumpPower = 15.0f;
}

MutantStateMoveEdge::MutantStateMoveEdge(Mutant& mutant) :
	MutantState(mutant)
{
}

MutantStateMoveEdge::~MutantStateMoveEdge()
{
}

void MutantStateMoveEdge::Enter()
{
	// ステージ中央からプレイヤーまでのベクトルを求める
	Vector3 playerPos = GetPlayer()->GetComponent<Transform>()->pos;
	Vector3 centerToPlayer = playerPos - kStageCenterPos;
	// ステージ中央から求めたベクトルの反対方向が移動する方向
	Vector3 moveDir = -centerToPlayer.Normalized();
	// ステージ中央から求めた方向に伸ばした先が移動する位置
	Vector3 targetPos = kStageCenterPos + moveDir * kStageRadius;

	// 現在位置から目標位置に毎フレームどれだけ移動するかを計算する
	Vector3 currentPos = m_owner.GetComponent<Transform>()->pos;
	m_moveVec = (targetPos - currentPos) / kMoveEdgeFrame;

	m_owner.GetComponent<Physics>()->m_vel.y = kJumpPower; // ジャンプみたいな動きさせる
}

void MutantStateMoveEdge::Update()
{
	m_frameCount++;

	if (m_frameCount <= kMoveEdgeFrame)
	{
		m_owner.GetComponent<Transform>()->pos += m_moveVec;
	}

	if (m_frameCount >= kStateEndFrame)
	{
		m_owner.GetComponent<StateMachine<Mutant>>()->ChangeState(std::make_unique<MutantStateCharge>(m_owner));
		return;
	}
}

void MutantStateMoveEdge::Exit()
{
}
