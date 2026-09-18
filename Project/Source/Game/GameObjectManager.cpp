#include "GameObjectManager.h"
#include "GameObjects/GameObject.h"

void GameObjectManager::Update()
{
	for (auto& obj : m_gameObjects)
	{
		obj->Update();
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
}
