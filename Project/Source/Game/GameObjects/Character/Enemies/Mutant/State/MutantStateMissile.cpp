#include "MutantStateMissile.h"
#include "../Mutant.h"
#include "Components/State/StateMachine.h"
#include "MutantStateOrbit.h"
#include "Game/GameObjects/Missile.h"
#include "Game/GameObjects/Character/Player/Player.h"
#include "Components/Transform.h"
#include "Game/GameObjectManager.h"

namespace
{
	constexpr int kMissileShotInterval = 60;
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
	//printfDx(L"MutantStateMissile::Enter()\n");
	//m_owner.GetComponent<StateMachine<Mutant>>()->ChangeState(std::make_unique<MutantStateOrbit>(m_owner));
}

void MutantStateMissile::Update()
{
	m_frameCount++;

	if (m_frameCount % kMissileShotInterval == 0)
	{
		Vector3 pos;
		pos = GetPlayer()->GetComponent<Transform>()->pos;
		pos.y = m_owner.GetComponent<Transform>()->pos.y + 1.0f;
		auto missile = std::make_shared<Missile>(pos);
		GetGameObjectManager()->Add(missile);
	}

	if (m_frameCount >= kMissileShotInterval * kMissileShotCount)
	{
		m_owner.GetComponent<StateMachine<Mutant>>()->ChangeState(std::make_unique<MutantStateOrbit>(m_owner));
		return;
	}
}

void MutantStateMissile::Exit()
{
}
