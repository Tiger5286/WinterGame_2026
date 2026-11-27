#include "MiniDrone.h"
#include "Resource/ResourceManager.h"
#include "Resource/Model.h"
#include "Resource/Graph.h"
#include "Character/Player/Player.h"
#include "Game/HackingManager.h"
#include "Components/Transform.h"
#include "Components/Animator/SimpleAnimator.h"
#include "System/Laser.h"
#include "Components/Hackable.h"

namespace
{
	const Vector3 kScale = Vector3::One() * 0.5f;

	const Vector3 kLocalPos = { 50,150,0 };
	const Vector3 kAimLocalpos = { 0,200,0 };
}

MiniDrone::MiniDrone(const std::shared_ptr<Player>& pPlayer, const std::shared_ptr<HackingManager>& pHackingManager) :
	m_pPlayer(pPlayer),
	m_pHackingManager(pHackingManager)
{
}

MiniDrone::~MiniDrone()
{
}

void MiniDrone::Init()
{
	m_transform->scale = kScale;

	// モデルを複製して取得
	m_pModel = ResourceManager::GetInstance().DuplicateModel(L"MiniDrone");

	// アニメーションを再生
	m_animator = AddComponent<SimpleAnimator>();
	m_animator->Init(m_pModel.get());
	m_animator->Play(L"Scene");

	// プレイヤーのnullチェック
	auto player = m_pPlayer.lock();
	if (!player)
	{
		assert(false && L"MiniDrone::Init() : プレイヤーがnullptrです");
		return;
	}
	else
	{
		// プレイヤーのTransformを取得
		m_playerTrans = player->GetComponent<Transform>();
	}
}

void MiniDrone::Update()
{
	auto player = m_pPlayer.lock();
	if (!player) return;

	Vector3 localPos = kLocalPos;
	if (player->IsAim())
	{
		localPos = kAimLocalpos;
	}
	Matrix4x4 rot = Matrix4x4::GetRotY(m_playerTrans->rot.y);
	localPos *= rot;

	m_pos = m_playerTrans->pos;
	m_pos += localPos;

	m_transform->pos.Lerp(m_pos, 0.2f);

	m_transform->rot.y = m_playerTrans->rot.y;
	m_pModel->SetTransform(*m_transform);

	// アニメーションを進める
	m_animator->Update(GetTimeScale());
}

void MiniDrone::Draw()
{
	// モデルを描画
	m_pModel->Draw();

	auto hm = m_pHackingManager.lock();
	if (!hm)
	{
		assert(false && "MiniDrone::Draw() : HackingManagerがnullptrです");
		return;
	}
	
	auto target = hm->GetTarget();
	if (!target) return;

	Vector3 targetPos = target->GetComponent<Transform>()->pos;
	Vector3 hackLocalPos = target->GetComponent<Hackable>()->GetHackLocalPos();
	targetPos += hackLocalPos;
	const int handle = ResourceManager::GetInstance().GetGraph(L"LaserTex")->GetHandle();
	auto color = GetColorU8(0, 255, 255, 255);

	Laser::Draw(m_transform->pos, targetPos, 15, handle, color);
}
