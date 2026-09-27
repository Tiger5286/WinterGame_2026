#pragma once
#include <list>
#include <memory>

class GameObject;

class CollisionManager
{
public:
	CollisionManager() = default;
	~CollisionManager() = default;

	void Update();
	void Draw();

	void Register(std::weak_ptr<GameObject> pObject);
	void UnRegister(std::weak_ptr<GameObject> pObject);

private:
	std::list<std::weak_ptr<GameObject>> m_pObjects;
};

