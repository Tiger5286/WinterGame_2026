#pragma once
#include "../GameObject.h"

class Model;

class StaticStage : public GameObject
{
public:
	StaticStage();
	~StaticStage();

	void Init() override;
	void Update() override;
	void Draw() override;

	void OnWasShot(const CollisionManager::ShotInfo& info) override;

private:
	Model* m_pModel = nullptr;
};

