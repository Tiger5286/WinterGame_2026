#include "CollisionManager.h"
#include "Game/GameObjects/GameObject.h"

void CollisionManager::Update()
{
	// TODO:死んだGameObjectを自動的に削除する機能を作る
}

void CollisionManager::Draw()
{
	for (auto& obj : m_pObjects)
	{
	}
}

void CollisionManager::Register(std::weak_ptr<GameObject> pObject)
{
	// TODO:listに引数のオブジェクトを追加する機能を作る
}

void CollisionManager::UnRegister(std::weak_ptr<GameObject> pObject)
{
	// TODO:listから引数のオブジェクトを除外する機能を作る
}