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

	m_pModel = ResourceManager::GetInstance().GetModel(L"TestStageModel");
	AddComponent<PolygonCollider>(*GetComponent<Transform>(), m_pModel);
}

void StaticStage::Update()
{
}

void StaticStage::Draw()
{
	GetComponent<PolygonCollider>()->Draw();
}

void StaticStage::OnWasShot()
{
	printfDx(L"StaticStageに当たった\n");
}
