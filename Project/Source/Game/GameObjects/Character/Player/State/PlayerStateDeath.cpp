#include "PlayerStateDeath.h"
#include "Components/Physics.h"
#include "../Player.h"

PlayerStateDeath::PlayerStateDeath(Player& player) :
	PlayerState(player)
{
}

PlayerStateDeath::~PlayerStateDeath()
{
}

void PlayerStateDeath::Enter()
{
	SetIsRun(false);
	// 死亡時に滑っていかないよう水平方向の加速を止める
	auto physics = m_owner.GetComponent<Physics>();
	physics->SetDrag(Physics::kDefaultDrag);
	physics->m_accel.x = 0.0f;
	physics->m_accel.z = 0.0f;
}

void PlayerStateDeath::Update()
{
	// 死亡後はどこにも遷移しない
}

void PlayerStateDeath::Exit()
{
}
