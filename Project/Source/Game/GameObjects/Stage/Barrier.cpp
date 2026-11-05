#include "Barrier.h"
#include "Resource/ResourceManager.h"
#include "Components/Collider/PolygonCollider.h"
#include "System/ServiceLocator.h"
#include "Game/CollisionManager.h"
#include "System/EventManager.h"
#include "Components/Transform.h"
#include "Resource/Model.h"

Barrier::Barrier(const std::wstring& modelName, ActionType defaultType) :
	m_modelName(modelName),
	m_defaultActionType(defaultType)
{
}

Barrier::~Barrier()
{
}

void Barrier::Init()
{
	m_actionMap[ActionType::Enable] = [this]() { GetComponent<PolygonCollider>()->SetEnable(true); };
	m_actionMap[ActionType::Disable] = [this]() { GetComponent<PolygonCollider>()->SetEnable(false); };
	m_actionMap[ActionType::Destroy] = [this]() { Destroy(); };

	m_pModel = ResourceManager::GetInstance().DuplicateModel(m_modelName);
	AddComponent<PolygonCollider>(*GetComponent<Transform>(), m_pModel.get());
	ServiceLocator::GetInstance().GetCollisionManager().Register(shared_from_this());

	m_actionMap[m_defaultActionType]();

	for (const auto& actionInfo : m_eventActionInfos)
	{
		EventManager::GetInstance().Register(actionInfo.eventName, m_actionMap[actionInfo.actionType], shared_from_this());
	}
}

void Barrier::Update()
{
}

void Barrier::Draw()
{
	GetComponent<PolygonCollider>()->Draw();
}

void Barrier::AddEvent(const ActionInfo& actionInfo)
{
	m_eventActionInfos.push_back(actionInfo);
}