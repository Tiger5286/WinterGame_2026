#include "Barrier.h"
#include "Resource/ResourceManager.h"
#include "Components/Collider/PolygonCollider.h"
#include "System/ServiceLocator.h"
#include "Game/CollisionManager.h"
#include "System/EventManager.h"
#include "Components/Transform.h"

Barrier::Barrier()
{
}

Barrier::~Barrier()
{
}

void Barrier::Init()
{
	ServiceLocator::GetInstance().GetCollisionManager().Register(shared_from_this());

	m_pModel = ResourceManager::GetInstance().GetModel(L"Barrier1");
	AddComponent<PolygonCollider>(*GetComponent<Transform>(), m_pModel);

	EventManager::GetInstance().Register(L"RemoveBarrier1", [this]() {Destroy(); },shared_from_this() );
}

void Barrier::Update()
{
	
}

void Barrier::Draw()
{
	GetComponent<PolygonCollider>()->Draw();
}

void Barrier::Destroy()
{
	m_isDead = true;
}