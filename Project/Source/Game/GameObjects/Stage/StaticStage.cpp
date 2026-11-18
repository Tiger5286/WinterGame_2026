#include "StaticStage.h"
#include "Resource/ResourceManager.h"
#include "DxLib.h"
#include "Resource/Model.h"
#include "Components/Collider/PolygonCollider.h"
#include "System/ServiceLocator.h"
#include "Game/CollisionManager.h"

StaticStage::StaticStage()
{	
}

StaticStage::~StaticStage()
{
}

void StaticStage::Init()
{
	ServiceLocator::GetInstance().GetCollisionManager().Register(shared_from_this());

	m_pModel = ResourceManager::GetInstance().GetModel(L"StaticStageModel");
	m_pVisualModel = ResourceManager::GetInstance().GetModel(L"StaticVisualStageModel");
	AddComponent<PolygonCollider>(*GetComponent<Transform>(), m_pModel);
}

void StaticStage::Update()
{
}

void StaticStage::Draw()
{
	if (!m_pModel)
	{
		assert(false && "StaticStage::Draw() : ステージの当たり判定モデルがnullptrです");
		return;
	}

	if (m_pVisualModel)
	{
		m_pVisualModel->Draw();
	}
	else
	{
		GetComponent<PolygonCollider>()->Draw();
	}
}

void StaticStage::OnWasShot(const CollisionManager::ShotInfo& info)
{
}
