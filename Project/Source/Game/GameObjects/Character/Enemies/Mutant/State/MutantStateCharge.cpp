#include "MutantStateCharge.h"
#include "../Mutant.h"
#include "Components/State/StateMachine.h"
#include "MutantStateOrbit.h"

MutantStateCharge::MutantStateCharge(Mutant& mutant) :
	MutantState(mutant)
{
}

MutantStateCharge::~MutantStateCharge()
{
}

void MutantStateCharge::Enter()
{
	printfDx(L"MutantStateCharge::Enter()\n");
	m_owner.GetComponent<StateMachine<Mutant>>()->ChangeState(std::make_unique<MutantStateOrbit>(m_owner));
}

void MutantStateCharge::Update()
{
}

void MutantStateCharge::Exit()
{
}
