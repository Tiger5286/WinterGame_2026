#include "TestEnemy.h"
#include "Resource/ResourceManager.h"
#include "Resource/Model.h"
#include "Components/Animator/Animator.h"
#include "Components/Collider/CapsuleCollider.h"
#include "Components/Physics.h"
#include "System/ServiceLocator.h"
#include "Game/CollisionManager.h"
#include "Components/Hitbox.h"
#include "Components/Hackable.h"
#include "Game/UI/EnemyHPUI.h"
#include "System/UIManager.h"

namespace
{
	constexpr const wchar_t* kAnimName = L"mixamo.com";

	constexpr float kColliderRadius = 30.0f;
	constexpr float kColliderHeight = 180.0f;

	const Vector3 kCenterOffset = Vector3(0.0f, 100.0f, 0.0f);

	const Vector3 kHPUIOffset = Vector3(0.0f, 200.0f, 0.0f);
}

TestEnemy::TestEnemy()
{
	AddComponent<Animator>();
	AddComponent<CapsuleCollider>(*GetComponent<Transform>(), kColliderRadius, kColliderHeight);
	AddComponent<Physics>();
	AddComponent<Hitbox>();
	AddComponent<Hackable>();
	m_tag = Tag::Enemy;
	m_centerOffset = kCenterOffset;
}

TestEnemy::~TestEnemy()
{
	m_pHPUI->Destroy();
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

	std::vector<Hitbox::Info> hitboxInfos;
	Hitbox::Info info;

	auto capsule = std::make_shared<CapsuleCollider>(*GetComponent<Transform>(), 20.0f, 180.0f);
	info.pCollider = capsule;
	hitboxInfos.push_back(info);

	GetComponent<Hitbox>()->Init(hitboxInfos);

	GetComponent<Hackable>()->Init();
	GetComponent<Hackable>()->SetFunc([this](Hackable::HackedData data) { OnHacked(data); });

	m_pHPUI = std::make_shared<EnemyHPUI>();
	UIManager::GetInstance().AddUI(m_pHPUI);
}

void TestEnemy::Update()
{
	GetComponent<Physics>()->Update();

	GetComponent<Animator>()->Update();

	GetComponent<Hackable>()->Update();

	bool isHacked = GetComponent<Hackable>()->IsHacked();
	if (isHacked)
	{
		MV1SetMaterialDifColor(m_pModel->GetHandle(), 0, GetColorF(0.0f, 1.0f, 1.0f, 1.0f));
	}
	else
	{
		MV1SetMaterialDifColor(m_pModel->GetHandle(), 0, GetColorF(0.8f, 0.8f, 0.8f, 1.0f));
	}

	EnemyHPUI::Info info;
	info.uiPos = GetComponent<Transform>()->pos + kHPUIOffset;
	info.nowHP = m_hp;
	info.maxHP = kMaxHP;
	m_pHPUI->SetInfo(info);

	m_pModel->SetTransform(*GetComponent<Transform>());
}

void TestEnemy::Draw()
{
	m_pModel->Draw();

#ifdef _DEBUG
	GetComponent<CapsuleCollider>()->Draw();
	GetComponent<Hitbox>()->Draw();
#endif
}

void TestEnemy::OnWasShot(const CollisionManager::ShotInfo& info)
{
	printfDx(L"TestEnemyに当たった\n");
	if (GetComponent<Hackable>()->IsHacked())
	{
		m_hp -= info.damage;
	}
	else
	{
		m_hp -= info.damage / 10;
	}

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_isDead = true;
	}
}

void TestEnemy::OnHacked(Hackable::HackedData data)
{
	printfDx(L"TestEnemyをハックした\n");
}
