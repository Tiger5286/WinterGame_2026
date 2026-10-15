#include "Walker.h"
#include "Components/Collider/CapsuleCollider.h"
#include "Components/Collider/SphereCollider.h"
#include "Components/Physics.h"
#include "Components/Hitbox/Hitbox.h"
#include "Components/Hackable.h"
#include "Components/State/StateMachine.h"
#include "Components/Transform.h"
#include "Components/Animator/Animator.h"
#include "System/ServiceLocator.h"
#include "Game/CollisionManager.h"
#include "State/WalkerStateApproach.h"
#include "Resource/ResourceManager.h"
#include "Resource/Model.h"
#include "Resource/Graph.h"
#include "Game/GameObjects/Character/Player/Player.h"
#include "Game/UI/EnemyHPUI.h"
#include "System/UIManager.h"
#include "State/WalkerStateDeath.h"

namespace
{
	constexpr float kColliderRadius = 25.0f;
	constexpr float kColliderHeight = 180.0f;

	const Vector3 kHackLocalPos = { 0.0f,100.0f,0.0f };

	const Vector3 kHPUIOffset = { 0.0f,200.0f,0.0f };
}

Walker::Walker(std::shared_ptr<Player> pPlayer) :
	EnemyBase(pPlayer)
{
	AddComponent<Physics>();
	AddComponent<CapsuleCollider>(*GetComponent<Transform>(), kColliderRadius, kColliderHeight);
	AddComponent<Hitbox>();
	AddComponent<Hackable>();
	AddComponent<StateMachine<Walker>>(*this);
	AddComponent<Animator>();
	m_tag = Tag::Enemy;
}

Walker::~Walker()
{
	m_pHPUI->Destroy();
}

void Walker::Init()
{
	ServiceLocator::GetInstance().GetCollisionManager().Register(shared_from_this());

	GetComponent<Physics>()->Init(GetComponent<Transform>(), GetComponent<CapsuleCollider>());

	m_pModel = ResourceManager::GetInstance().DuplicateModel(L"Walker");

	auto animator = GetComponent<Animator>();
	animator->Init(m_pModel.get());
	animator->AddAnimation(L"Walker|Walk");
	animator->AddAnimation(L"Walker|LeftWalk");
	animator->AddAnimation(L"Walker|Attack", 0.5f, false);
	animator->AddAnimation(L"Walker|Death", 0.5f, false);

	auto stateMachine = GetComponent<StateMachine<Walker>>();
	stateMachine->ChangeState(std::make_unique<WalkerStateApproach>(*this));

	Hackable::Info hackableInfo;
	hackableInfo.func = [this](Hackable::HackedData data) { OnHacked(data); };
	hackableInfo.hackLocalPos = kHackLocalPos;
	GetComponent<Hackable>()->SetInfo(hackableInfo);
	GetComponent<Hackable>()->Init();

	std::vector<Hitbox::Info> hitboxInfoArray;
	Hitbox::Info hitboxInfo;
	hitboxInfo.pCollider = std::make_shared<CapsuleCollider>(*GetComponent<Transform>(), 20, 155);
	hitboxInfoArray.push_back(hitboxInfo);
	hitboxInfo.pCollider = std::make_shared<SphereCollider>(*GetComponent<Transform>(), 15);
	hitboxInfo.pCollider->SetOffset(Vector3(0, 165, 0));
	hitboxInfo.isWeakPoint = true;
	hitboxInfoArray.push_back(hitboxInfo);
	GetComponent<Hitbox>()->Init(hitboxInfoArray);

	m_hp = kMaxHp;
	m_pHPUI = std::make_shared<EnemyHPUI>();
	UIManager::GetInstance().AddUI(m_pHPUI);
}

void Walker::Update()
{
	auto& resourceManager = ResourceManager::GetInstance();

	GetComponent<StateMachine<Walker>>()->Update();
	m_attackCooltime--;
	if (m_attackCooltime < 0) m_attackCooltime = 0;

	GetComponent<Physics>()->Update();

	auto hackable = GetComponent<Hackable>();
	hackable->Update();
	if (!hackable->IsHacked())
	{
		// テクスチャを通常に戻す
		MV1SetTextureGraphHandle(m_pModel->GetHandle(), 0, resourceManager.GetGraph(L"NormalWalkerTex")->GetHandle(), false);
		MV1SetMaterialEmiColor(m_pModel->GetHandle(), 0, GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
	}

	auto transform = GetComponent<Transform>();
	m_pModel->SetTransform(*transform);

	GetComponent<Animator>()->Update();

	EnemyHPUI::Info info;
	info.uiPos = GetComponent<Transform>()->pos + kHPUIOffset;
	info.nowHP = m_hp;
	info.maxHP = kMaxHp;
	m_pHPUI->SetInfo(info);
}

void Walker::Draw()
{
	m_pModel->Draw();
	GetComponent<StateMachine<Walker>>()->Draw();
#ifdef _DEBUG
	GetComponent<CapsuleCollider>()->Draw();
	GetComponent<Hitbox>()->Draw();
#endif
}

void Walker::OnWasShot(const CollisionManager::ShotInfo& info)
{
	printfDx(L"Walkerに当たった\n");
	if (GetComponent<Hackable>()->IsHacked())
	{
		if (info.isWeakPoint)
		{
			printfDx(L"弱点！\n");
			m_hp -= info.damage * 1.5f;
		}
		else
		{
			m_hp -= info.damage;
		}
	}
	else
	{
		m_hp -= info.damage / 10;
	}

	if (m_hp <= 0)
	{
		m_hp = 0;
		GetComponent<StateMachine<Walker>>()->ChangeState(std::make_unique<WalkerStateDeath>(*this));
	}
}

void Walker::OnHacked(Hackable::HackedData data)
{
	auto& resourceManager = ResourceManager::GetInstance();
	printfDx(L"Walkerがハックされた\n");
	// テクスチャをハックされたものに変える
	MV1SetTextureGraphHandle(m_pModel->GetHandle(), 0, resourceManager.GetGraph(L"HackedWalkerTex")->GetHandle(), false);
	MV1SetMaterialEmiColor(m_pModel->GetHandle(), 0, GetColorF(0.5f,0.5f,0.5f,1.0f));
}

void Walker::RotToPlayer()
{
	auto transform = GetComponent<Transform>();
	auto playerTrans = m_pPlayer.lock()->GetComponent<Transform>();

	auto toPlayerVec = playerTrans->pos - transform->pos;
	transform->rot.y = toPlayerVec.Angle();
}
