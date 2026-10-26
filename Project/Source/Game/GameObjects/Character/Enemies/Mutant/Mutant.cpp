#include "Mutant.h"

Mutant::Mutant(std::shared_ptr<Player> pPlayer) :
	EnemyBase(pPlayer)
{
}

Mutant::~Mutant()
{
}

void Mutant::Init()
{
}

void Mutant::Update()
{
}

void Mutant::Draw()
{
}

void Mutant::OnWasShot(const CollisionManager::ShotInfo& info)
{
}
