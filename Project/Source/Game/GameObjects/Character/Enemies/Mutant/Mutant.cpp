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
#ifdef _DEBUG
#include "Game.h"
#include "State/MutantStateAttack.h"
#include "State/MutantStateBackStep.h"
#include "State/MutantStateCharge.h"
#include "State/MutantStateMissile.h"
#include "State/MutantStateOrbit.h"
#endif

namespace
{
	constexpr float kColliderRadius = 100.0f;
	constexpr float kColliderHeight = 400.0f;

	constexpr float kModelScale = 2.5f;

	const Vector3 kHPUIOffset = { 0.0f,450.0f,0.0f };

	const Vector3 kHackLocalPos = { 0.0f,250.0f,0.0f };
	constexpr const wchar_t* kHackBoardPath = L"data/hackBoard/mutantBoard.csv";

#ifdef _DEBUG
	// デバッグ用のステート切り替えに必要な情報
	struct DebugStateData
	{
		int key;
		const wchar_t* keyName;
		const wchar_t* stateName;
		MutantState::ID id;
		std::unique_ptr<State<Mutant>>(*create)(Mutant&);
	};

	template<class T>
	std::unique_ptr<State<Mutant>> CreateDebugState(Mutant& mutant)
	{
		return std::make_unique<T>(mutant);
	}

	const DebugStateData kDebugStates[] = {
		{ KEY_INPUT_1, L"1", L"接近", MutantState::ID::Approach, CreateDebugState<MutantStateApproach> },
		{ KEY_INPUT_2, L"2", L"通常攻撃", MutantState::ID::Attack, CreateDebugState<MutantStateAttack> },
		{ KEY_INPUT_3, L"3", L"警戒", MutantState::ID::Orbit, CreateDebugState<MutantStateOrbit> },
		{ KEY_INPUT_4, L"4", L"バックステップ", MutantState::ID::BackStep, CreateDebugState<MutantStateBackStep> },
		{ KEY_INPUT_5, L"5", L"突進", MutantState::ID::Charge, CreateDebugState<MutantStateCharge> },
		{ KEY_INPUT_6, L"6", L"ミサイル", MutantState::ID::Missile, CreateDebugState<MutantStateMissile> },
		{ KEY_INPUT_7, L"7", L"死亡", MutantState::ID::Death, CreateDebugState<MutantStateDeath> },
	};

	// 警戒はクールタイムが0だとすぐ終わるので、デバッグで切り替えたときはこの時間だけ続ける
	constexpr int kDebugOrbitCooltime = 180;

	const Vector3 kDebugStateNameOffset = { 0.0f,520.0f,0.0f };

	constexpr int kDebugPanelWidth = 220;
	constexpr int kDebugPanelMargin = 10;
	constexpr int kDebugPanelPadding = 8;
	constexpr int kDebugLineHeight = 18;
	constexpr int kDebugPanelAlpha = 160;
	constexpr unsigned int kDebugTextColor = 0xffffff;
	constexpr unsigned int kDebugCurrentColor = 0xffff00;
#endif
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
#ifdef _DEBUG
	DebugChangeState();
#endif
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
	DebugDraw();
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

#ifdef _DEBUG
void Mutant::DebugChangeState()
{
	// 押されているキーを探す
	const DebugStateData* pPressed = nullptr;
	for (const auto& data : kDebugStates)
	{
		if (CheckHitKey(data.key))
		{
			pPressed = &data;
			break;
		}
	}

	// 押した瞬間だけ切り替える
	const int key = pPressed ? pPressed->key : -1;
	const bool isTrigger = key != m_debugPrevKey;
	m_debugPrevKey = key;
	if (!pPressed || !isTrigger) return;

	// 死亡ステートではコライダーなどを無効にしているので、他のステートに戻さない
	if (IsDying()) return;

	GetComponent<StateMachine<Mutant>>()->ChangeState(pPressed->create(*this));

	// 前のステートのExit()でクールタイムが上書きされるので、切り替えの後で設定する
	if (pPressed->id == MutantState::ID::Orbit)
	{
		m_attackCooltime = kDebugOrbitCooltime;
	}
}

void Mutant::DebugDraw()
{
	// 現在のステートの名前を探す
	const MutantState* pState = GetComponent<StateMachine<Mutant>>()->GetState<MutantState>();
	std::wstring currentName = L"なし";
	for (const auto& data : kDebugStates)
	{
		if (pState && pState->GetID() == data.id)
		{
			currentName = data.stateName;
			break;
		}
	}

	// ボスの頭上にステートの名前を描画
	const Vector3 namePos = GetComponent<Transform>()->pos + kDebugStateNameOffset;
	if (!CheckCameraViewClip(namePos))
	{
		const Vector3 screenPos = Vector3::FromDxLib(ConvWorldPosToScreenPos(namePos));
		const int width = GetDrawStringWidth(currentName.c_str(), static_cast<int>(currentName.size()));
		DrawString(static_cast<int>(screenPos.x) - width / 2, static_cast<int>(screenPos.y), currentName.c_str(), kDebugCurrentColor);
	}

	// 画面右上に半透明のパネルを描画(タイトル、現在のステート、クールタイム + ステートの一覧)
	const int stateNum = static_cast<int>(std::size(kDebugStates));
	const int panelLeft = Game::kScreenWidth - kDebugPanelWidth - kDebugPanelMargin;
	const int panelTop = kDebugPanelMargin;
	const int panelBottom = panelTop + kDebugPanelPadding * 2 + kDebugLineHeight * (3 + stateNum);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, kDebugPanelAlpha);
	DrawBox(panelLeft, panelTop, panelLeft + kDebugPanelWidth, panelBottom, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	const int textX = panelLeft + kDebugPanelPadding;
	int textY = panelTop + kDebugPanelPadding;
	DrawString(textX, textY, L"Mutant ステート切り替え", kDebugTextColor);
	textY += kDebugLineHeight;
	DrawFormatString(textX, textY, kDebugTextColor, L"現在 : %s", currentName.c_str());
	textY += kDebugLineHeight;
	DrawFormatString(textX, textY, kDebugTextColor, L"クールタイム : %d", m_attackCooltime);
	textY += kDebugLineHeight;

	// 現在のステートの行は色を変える
	for (const auto& data : kDebugStates)
	{
		const bool isCurrent = pState && pState->GetID() == data.id;
		DrawFormatString(textX, textY, isCurrent ? kDebugCurrentColor : kDebugTextColor, L"%s%s : %s", isCurrent ? L"> " : L"  ", data.keyName, data.stateName);
		textY += kDebugLineHeight;
	}
}
#endif
