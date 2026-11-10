#include "MutantStateMoveEdge.h"
#include "Utility/Vector3.h"
#include "Game/GameObjects/Character/Player/Player.h"
#include "Components/Transform.h"

namespace
{
	// ステージの中心位置
	const Vector3 kStageCenterPos = Vector3(6750, 100, 250);
	// ステージの中心から端の距離
	constexpr float kStageRadius = 1000.0f;

	// 端に移動する時間
	constexpr int kMoveEdgeFrame = 30;
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
	m_targetPos = kStageCenterPos + moveDir * kStageRadius;

	
}

void MutantStateMoveEdge::Update()
{
}

void MutantStateMoveEdge::Exit()
{
}
