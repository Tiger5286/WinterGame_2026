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

	void OnWasShot() override;

private:
	Model* m_pModel = nullptr;
};

