#include "WalkerStateDeath.h"
#include "Components/Animator/Animator.h"
#include "Components/Physics.h"
#include "../Walker.h"

WalkerStateDeath::WalkerStateDeath(Walker& walker) :
	WalkerState(walker)
{
}

WalkerStateDeath::~WalkerStateDeath()
{
}

void WalkerStateDeath::Enter()
{
	m_owner.GetComponent<Physics>()->m_accel = Vector3::Zero();
	m_owner.GetComponent<Animator>()->Play(L"Walker|Death");
}

void WalkerStateDeath::Update()
{
	if (m_owner.GetComponent<Animator>()->IsEnd())
	{
		Die();
	}
}

void WalkerStateDeath::Exit()
{
}
