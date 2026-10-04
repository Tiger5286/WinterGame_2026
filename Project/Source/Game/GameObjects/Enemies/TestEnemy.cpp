#include "TestEnemy.h"
#include "Resource/ResourceManager.h"
#include "Resource/Model.h"
#include "Components/Animator/Animator.h"
#include "Components/Collider/CapsuleCollider.h"
#include "Components/Physics.h"
#include "System/ServiceLocator.h"
#include "Game/CollisionManager.h"

namespace
{
	constexpr const wchar_t* kAnimName = L"mixamo.com";

	constexpr float kColliderRadius = 30.0f;
	constexpr float kColliderHeight = 180.0f;
}

TestEnemy::TestEnemy()
{
	AddComponent<Animator>();
	AddComponent<CapsuleCollider>(*GetComponent<Transform>(), kColliderRadius, kColliderHeight);
	AddComponent<Physics>();
}

TestEnemy::~TestEnemy()
{
}

void TestEnemy::Init()
{
	ServiceLocator::GetInstance().GetCollisionManager().Register(shared_from_this());

	m_pModel = ResourceManager::GetInstance().DuplicateModel(L"TestEnemyModel");

	GetComponent<Physics>()->Init(GetComponent<Transform>(), GetComponent<CapsuleCollider>());

	auto animator = GetComponent<Animator>();
	animator->Init(m_pModel.get());
	animator->AddAnimation(kAnimName);
	animator->Play(kAnimName);
}

void TestEnemy::Update()
{
	GetComponent<Physics>()->Update();

	GetComponent<Animator>()->Update();
}

void TestEnemy::Draw()
{
	m_pModel->Draw();

#ifdef _DEBUG
	GetComponent<CapsuleCollider>()->Draw();
#endif
}
