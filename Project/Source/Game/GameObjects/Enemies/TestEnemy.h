#pragma once
#include "EnemyBase.h"
#include "Components/Hackable.h"
#include <memory>

class Model;
class EnemyHPUI;

class TestEnemy : public EnemyBase
{
public:
	static constexpr int kMaxHP = 500;
public:
	TestEnemy(std::shared_ptr<Player> pPlayer);
	~TestEnemy() override;

	void Init() override;
	void Update() override;
	void Draw() override;

	void OnWasShot(const CollisionManager::ShotInfo& info) override;

private:
	void OnHacked(Hackable::HackedData data);

private:
	std::unique_ptr<Model> m_pModel;
	std::shared_ptr<EnemyHPUI> m_pHPUI;
	int m_hp = kMaxHP;
};