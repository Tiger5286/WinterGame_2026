#pragma once

class CollisionManager;

class ServiceLocator
{
public:
	static ServiceLocator& GetInstance();
	~ServiceLocator() = default;

private:
	ServiceLocator(const ServiceLocator&) = delete;
	ServiceLocator& operator=(const ServiceLocator&) = delete;
	ServiceLocator() = default;

public:

	void ProvideCollisionManager(CollisionManager* manager) { m_pCollisionManager = manager; }
	CollisionManager& GetCollisionManager() const;

private:
	CollisionManager* m_pCollisionManager = nullptr;
};