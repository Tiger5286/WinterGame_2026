#pragma once
#include <list>
#include <memory>

class GameObject;

class GameObjectManager
{
public:
	GameObjectManager() = default;
	~GameObjectManager() = default;

	void Update();
	void Draw() const;

	void Add(std::shared_ptr<GameObject> gameObject);

private:
	std::list<std::shared_ptr<GameObject>> m_gameObjects;
};

