#pragma once
#include "../GameObject.h"

class Model;

class StaticStage : public GameObject
{
public:
	StaticStage() = default;
	~StaticStage() = default;

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	Model* m_pModel;
};

