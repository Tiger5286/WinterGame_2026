#pragma once
#include "Game/GameObjects/GameObject.h"
#include "Components/Hackable.h"
#include <memory>

class Model;

class TestEnemy : public GameObject
{
public:
	TestEnemy();
	~TestEnemy() override;

	void Init() override;
	void Update() override;
	void Draw() override;

	void OnWasShot() override;

private:
	void OnHacked(Hackable::HackedData data);

private:
	std::unique_ptr<Model> m_pModel;
};