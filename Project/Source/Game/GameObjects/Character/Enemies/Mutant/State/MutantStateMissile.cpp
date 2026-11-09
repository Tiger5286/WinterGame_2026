#include "MutantStateMissile.h"
#include "../Mutant.h"
#include "Components/State/StateMachine.h"
#include "MutantStateOrbit.h"

MutantStateMissile::MutantStateMissile(Mutant& mutant) :
	MutantState(mutant)
{
}

MutantStateMissile::~MutantStateMissile()
{
}

void MutantStateMissile::Enter()
{
	printfDx(L"MutantStateMissile::Enter()\n");
	m_owner.GetComponent<StateMachine<Mutant>>()->ChangeState(std::make_unique<MutantStateOrbit>(m_owner));
}

void MutantStateMissile::Update()
{
}

void MutantStateMissile::Exit()
{
}
