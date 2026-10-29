#include "Mutant.h"
#include "Resource/ResourceManager.h"
#include "Resource/Model.h"
#include "Resource/Graph.h"
#include "Components/Physics.h"
#include "Components/Collider/CapsuleCollider.h"
#include "Components/Animator/Animator.h"
#include "Components/Hitbox/Hitbox.h"
#include "Components/Hitbox/HitboxSphere.h"
#include "Components/Hitbox/HitboxCapsule.h"
#include "Components/State/StateMachine.h"
#include "Components/Transform.h"
#include "System/ServiceLocator.h"
#include "System/UIManager.h"
#include "Game/CollisionManager.h"
#include "Game/UI/EnemyHPUI.h"
#include "Game/GameObjects/Character/Player/Player.h"
#include "State/MutantStateApproach.h"
#include "State/MutantStateDeath.h"
#include <string>

namespace
{
	constexpr float kColliderRadius = 100.0f;
	constexpr float kColliderHeight = 400.0f;

	constexpr float kModelScale = 2.5f;

	const Vector3 kHPUIOffset = { 0.0f,450.0f,0.0f };

	const Vector3 kHackLocalPos = { 0.0f,250.0f,0.0f };
	constexpr const wchar_t* kHackBoardPath = L"data/hackBoard/mutantBoard.csv";
}

Mutant::Mutant(std::shared_ptr<Player> pPlayer) :
	EnemyBase(pPlayer)
{
	AddComponent<Physics>();
	AddComponent<CapsuleCollider>(*GetComponent<Transform>(), kColliderRadius, kColliderHeight);
	AddComponent<Hitbox>();
	AddComponent<Hackable>();
	AddComponent<StateMachine<Mutant>>(*this);
	AddComponent<Animator>();
	m_tag = Tag::Enemy;
}

Mutant::~Mutant()
{
	m_pHPUI->Destroy();
}

void Mutant::Init()
{
	ServiceLocator::GetInstance().GetCollisionManager().Register(shared_from_this());

	m_pModel = ResourceManager::GetInstance().DuplicateModel(L"Mutant");
	auto transform = GetComponent<Transform>();
	transform->scale = Vector3::One() * kModelScale;

	GetComponent<Physics>()->Init(transform, GetComponent<CapsuleCollider>());

	auto animator = GetComponent<Animator>();
	animator->Init(m_pModel.get());
	animator->AddAnimation(L"Mutant|Idle");
	animator->AddAnimation(L"Mutant|WalkForward");
	animator->AddAnimation(L"Mutant|Attack", 0.5f, false);
	animator->AddAnimation(L"Mutant|Death", 0.5f, false);

	auto stateMachine = GetComponent<StateMachine<Mutant>>();
	stateMachine->ChangeState(std::make_unique<MutantStateApproach>(*this));

	Hackable::Info hackableInfo;
	hackableInfo.func = [this](Hackable::HackedData data) { OnHacked(data); };
	hackableInfo.hackLocalPos = kHackLocalPos;
	GetComponent<Hackable>()->SetInfo(hackableInfo);
	GetComponent<Hackable>()->LoadData(kHackBoardPath);
	GetComponent<Hackable>()->Init();

	// 差し替えるテクスチャ(名前にdiffuseを含むもの)の番号を探す
	for (int i = 0; i < MV1GetTextureNum(m_pModel->GetHandle()); i++)
	{
		const std::wstring texName = MV1GetTextureName(m_pModel->GetHandle(), i);
		if (texName.find(L"diffuse") != std::wstring::npos)
		{
			m_diffuseTexIndex = i;
			break;
		}
	}

	SetHitbox();

	m_hp = kMaxHp;
	m_pHPUI = std::make_shared<EnemyHPUI>();
	UIManager::GetInstance().AddUI(m_pHPUI);
}

void Mutant::Update()
{
	GetComponent<StateMachine<Mutant>>()->Update();
	m_attackCooltime--;
	if (m_attackCooltime < 0) m_attackCooltime = 0;

	// 死亡中はコライダーが無効で床と押し戻しされないため、物理を止める
	if (!IsDying())
	{
		GetComponent<Physics>()->Update();
	}

	auto hackable = GetComponent<Hackable>();
	hackable->Update();
	if (!hackable->IsHacked())
	{
		// テクスチャを通常に戻す
		MV1SetTextureGraphHandle(m_pModel->GetHandle(), m_diffuseTexIndex, ResourceManager::GetInstance().GetGraph(L"NormalMutantTex")->GetHandle(), false);
		MV1SetMaterialEmiColor(m_pModel->GetHandle(), 0, GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
	}

	auto transform = GetComponent<Transform>();
	m_pModel->SetTransform(*transform);

	GetComponent<Animator>()->Update();

	EnemyHPUI::Info info;
	info.uiPos = transform->pos + kHPUIOffset;
	info.nowHP = m_hp;
	info.maxHP = kMaxHp;
	m_pHPUI->SetInfo(info);

	GetComponent<Hitbox>()->Update();
}

void Mutant::Draw()
{
	m_pModel->Draw();
	GetComponent<StateMachine<Mutant>>()->Draw();
#ifdef _DEBUG
	GetComponent<CapsuleCollider>()->Draw();
	GetComponent<Hitbox>()->Draw();
#endif
}

void Mutant::OnWasShot(const CollisionManager::ShotInfo& info)
{
	// 死亡中は撃たれても何もしない(死亡ステートに入り直さない)
	if (IsDying()) return;

	// ハッキングしていないときはダメージが1/10になる(Walkerと同じ)
	if (GetComponent<Hackable>()->IsHacked())
	{
		if (info.isWeakPoint)
		{
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
		GetComponent<StateMachine<Mutant>>()->ChangeState(std::make_unique<MutantStateDeath>(*this));
	}
}

bool Mutant::IsDying()
{
	return GetComponent<StateMachine<Mutant>>()->GetState<MutantStateDeath>() != nullptr;
}

void Mutant::OnHacked(Hackable::HackedData data)
{
	// テクスチャをハックされたものに変える
	MV1SetTextureGraphHandle(m_pModel->GetHandle(), m_diffuseTexIndex, ResourceManager::GetInstance().GetGraph(L"HackedMutantTex")->GetHandle(), false);
	MV1SetMaterialEmiColor(m_pModel->GetHandle(), 0, GetColorF(0.5f, 0.5f, 0.5f, 1.0f));
}

void Mutant::RotToPlayer()
{
	auto transform = GetComponent<Transform>();
	auto playerTrans = m_pPlayer.lock()->GetComponent<Transform>();

	auto toPlayerVec = playerTrans->pos - transform->pos;
	transform->rot.y = toPlayerVec.Angle();
}

void Mutant::SetHitbox()
{
	// ヒットボックスの初期化(モデルを2.5倍しているので半径も大きめにする)
	auto hitbox = GetComponent<Hitbox>();
	// 頭
	HitboxSphere::Info sphere;
	sphere.frameName = L"mixamorig:Head";
	sphere.radius = 40.0f;
	sphere.pModel = m_pModel.get();
	sphere.isWeakPoint = true;
	hitbox->AddCollider(sphere);
	// 胴
	HitboxCapsule::Info capsule;
	capsule.bottomFrameName = L"mixamorig:Hips";
	capsule.topFrameName = L"mixamorig:Neck";
	capsule.radius = 60.0f;
	capsule.pModel = m_pModel.get();
	hitbox->AddCollider(capsule);
	// 右腕→肘
	capsule.bottomFrameName = L"mixamorig:RightArm";
	capsule.topFrameName = L"mixamorig:RightForeArm";
	capsule.radius = 25.0f;
	hitbox->AddCollider(capsule);
	// 右肘→手
	capsule.bottomFrameName = L"mixamorig:RightForeArm";
	capsule.topFrameName = L"mixamorig:RightHand";
	hitbox->AddCollider(capsule);
	// 左腕→肘
	capsule.bottomFrameName = L"mixamorig:LeftArm";
	capsule.topFrameName = L"mixamorig:LeftForeArm";
	hitbox->AddCollider(capsule);
	// 左肘→手
	capsule.bottomFrameName = L"mixamorig:LeftForeArm";
	capsule.topFrameName = L"mixamorig:LeftHand";
	hitbox->AddCollider(capsule);
	// 右脚→膝
	capsule.bottomFrameName = L"mixamorig:RightUpLeg";
	capsule.topFrameName = L"mixamorig:RightLeg";
	capsule.radius = 30.0f;
	hitbox->AddCollider(capsule);
	// 右膝→足
	capsule.bottomFrameName = L"mixamorig:RightLeg";
	capsule.topFrameName = L"mixamorig:RightFoot";
	capsule.radius = 25.0f;
	hitbox->AddCollider(capsule);
	// 左脚→膝
	capsule.bottomFrameName = L"mixamorig:LeftUpLeg";
	capsule.topFrameName = L"mixamorig:LeftLeg";
	capsule.radius = 30.0f;
	hitbox->AddCollider(capsule);
	// 左膝→足
	capsule.bottomFrameName = L"mixamorig:LeftLeg";
	capsule.topFrameName = L"mixamorig:LeftFoot";
	capsule.radius = 25.0f;
	hitbox->AddCollider(capsule);
}
