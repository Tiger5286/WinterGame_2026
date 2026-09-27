#include "CollisionManager.h"
#include "Game/GameObjects/GameObject.h"

void CollisionManager::Update()
{
	// TODO:死んだGameObjectを自動的に削除する機能を作る
}

void CollisionManager::Draw()
{
	// TODO:GameObjectのColliderを全部描画する
}

void CollisionManager::Register(const std::shared_ptr<GameObject>& pObject)
{
	// TODO:listに引数のオブジェクトを追加する機能を作る
}

void CollisionManager::UnRegister(const std::shared_ptr<GameObject>& pObject)
{
	// TODO:listから引数のオブジェクトを除外する機能を作る
}