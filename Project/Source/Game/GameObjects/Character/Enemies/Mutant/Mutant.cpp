#include "Mutant.h"
#include "Resource/ResourceManager.h"
#include "Resource/Model.h"

Mutant::Mutant(std::shared_ptr<Player> pPlayer) :
	EnemyBase(pPlayer)
{
}

Mutant::~Mutant()
{
}

void Mutant::Init()
{
	m_pModel = ResourceManager::GetInstance().DuplicateModel(L"Mutant");
}

void Mutant::Update()
{
	m_pModel->SetTransform(*GetComponent<Transform>());
}

void Mutant::Draw()
{
	m_pModel->Draw();
}

void Mutant::OnWasShot(const CollisionManager::ShotInfo& info)
{
}
