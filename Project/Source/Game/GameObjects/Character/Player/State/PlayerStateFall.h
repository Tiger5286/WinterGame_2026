#pragma once
#include "PlayerState.h"

class PlayerStateFall : public PlayerState
{
public:
	PlayerStateFall(Player& player);

	void Enter() override;
	void Update() override;
	void Exit() override;

	ID GetID() const override;
};
