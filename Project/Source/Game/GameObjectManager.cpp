#include "GameObjectManager.h"
#include "GameObjects/GameObject.h"

void GameObjectManager::Update()
{
	std::list<std::shared_ptr<GameObject>> deadObjects;
	for (auto& obj : m_gameObjects)
	{
		obj->Update();
		if (obj->IsDead())
		{
			deadObjects.push_back(obj);
		}
	}
	for (auto& obj : deadObjects)
	{
		m_gameObjects.remove(obj);
	}
}

void GameObjectManager::Draw() const
{
	for (const auto& obj : m_gameObjects)
	{
		obj->Draw();
	}
}

void GameObjectManager::Add(std::shared_ptr<GameObject> gameObject)
{
	gameObject->Init();
	m_gameObjects.push_back(gameObject);
	gameObject->SetId(m_newId);
	m_newId++;
}

std::list<std::weak_ptr<GameObject>> GameObjectManager::GetHackables() const
{
	std::list<std::weak_ptr<GameObject>> hackables;

	for (auto& obj : m_gameObjects)
	{
		if (obj->IsHackable())
		{
			hackables.push_back(obj);
		}
	}

	return hackables;
}
