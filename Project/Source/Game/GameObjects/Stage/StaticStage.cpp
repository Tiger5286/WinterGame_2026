#include "StaticStage.h"
#include "Resource/ResourceManager.h"
#include "DxLib.h"
#include "Resource/Model.h"

void StaticStage::Init()
{
	m_pModel = ResourceManager::GetInstance().GetModel(L"TestStageModel");
}

void StaticStage::Update()
{
}

void StaticStage::Draw()
{
	MV1DrawModel(m_pModel->GetHandle());
}