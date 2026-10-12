#include "Walker.h"
#include "Components/Collider/CapsuleCollider.h"
#include "Components/Collider/SphereCollider.h"
#include "Components/Physics.h"
#include "Components/Hitbox.h"
#include "Components/Hackable.h"
#include "Components/State/StateMachine.h"
#include "Components/Transform.h"
#include "System/ServiceLocator.h"
#include "Game/CollisionManager.h"
#include "State/WalkerStateApproach.h"

namespace
{
	constexpr float kColliderRadius = 25.0f;
	constexpr float kColliderHeight = 180.0f;

	const Vector3 kHackLocalPos = { 0.0f,100.0f,0.0f };
}

Walker::Walker(std::shared_ptr<Player> pPlayer) :
	EnemyBase(pPlayer)
{
	AddComponent<Physics>();
	AddComponent<CapsuleCollider>(*GetComponent<Transform>(), kColliderRadius, kColliderHeight);
	AddComponent<Hitbox>();
	AddComponent<Hackable>();
	AddComponent<StateMachine<Walker>>(*this);
}

Walker::~Walker()
{
}

void Walker::Init()
{
	ServiceLocator::GetInstance().GetCollisionManager().Register(shared_from_this());

	GetComponent<Physics>()->Init(GetComponent<Transform>(), GetComponent<CapsuleCollider>());

	auto stateMachine = GetComponent<StateMachine<Walker>>();
	stateMachine->ChangeState(std::make_unique<WalkerStateApproach>(*this));

	Hackable::Info hackableInfo;
	hackableInfo.func = [this](Hackable::HackedData data) {OnHacked(); };
	hackableInfo.hackLocalPos = kHackLocalPos;
	GetComponent<Hackable>()->SetInfo(hackableInfo);
	GetComponent<Hackable>()->Init();

	std::vector<Hitbox::Info> hitboxInfoArray;
	Hitbox::Info hitboxInfo;
	hitboxInfo.pCollider = std::make_shared<CapsuleCollider>(*GetComponent<Transform>(), 20, 150);
	hitboxInfoArray.push_back(hitboxInfo);
	hitboxInfo.pCollider = std::make_shared<SphereCollider>(*GetComponent<Transform>(), 20);
	hitboxInfo.pCollider->SetOffset(Vector3(0, 150, 0));
	hitboxInfo.isWeakPoint = true;
	hitboxInfoArray.push_back(hitboxInfo);
	GetComponent<Hitbox>()->Init(hitboxInfoArray);
}

void Walker::Update()
{
	GetComponent<StateMachine<Walker>>()->Update();

	GetComponent<Physics>()->Update();

	GetComponent<Hackable>()->Update();
}

void Walker::Draw()
{
#ifdef _DEBUG
	GetComponent<CapsuleCollider>()->Draw();
	GetComponent<Hitbox>()->Draw();
#endif
}

void Walker::OnHacked()
{
	printfDx(L"Walkerがハックされた\n");
}
