#pragma once
#include "PlayerState.h"

class PlayerStateLand : public PlayerState
{
public:
	PlayerStateLand(Player& player);

	void Enter() override;
	void Update() override;
	void Exit() override;

	ID GetID() const override;

private:
	float m_frame = 0.0f;
};
