#include "Mutant.h"
#include "Resource/ResourceManager.h"
#include "Resource/Model.h"
#include "Components/Physics.h"
#include "Components/Collider/CapsuleCollider.h"
#include "Components/Animator/Animator.h"
#include "System/ServiceLocator.h"
#include "Game/CollisionManager.h"

Mutant::Mutant(std::shared_ptr<Player> pPlayer) :
	EnemyBase(pPlayer)
{
	AddComponent<Physics>();
	AddComponent<CapsuleCollider>(*GetComponent<Transform>(), 100, 400);
}

Mutant::~Mutant()
{
} 

void Mutant::Init()
{
	ServiceLocator::GetInstance().GetCollisionManager().Register(shared_from_this());

	m_pModel = ResourceManager::GetInstance().DuplicateModel(L"Mutant");
	auto transform = GetComponent<Transform>();
	transform->scale = Vector3::One() * 2.5f;

	GetComponent<Physics>()->Init(transform, GetComponent<CapsuleCollider>());
}

void Mutant::Update()
{
	m_pModel->SetTransform(*GetComponent<Transform>());
}

void Mutant::Draw()
{
	m_pModel->Draw();
#ifdef _DEBUG
	GetComponent<CapsuleCollider>()->Draw();
#endif
}

void Mutant::OnWasShot(const CollisionManager::ShotInfo& info)
{
}
