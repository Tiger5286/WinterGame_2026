#pragma once
#include "PlayerState.h"

class PlayerStateMove : public PlayerState
{
public:
	PlayerStateMove(Player& owner);

	void Enter() override;
	void Update() override;
	void Exit() override;

	ID GetID() const override { return ID::Move; }
private:

};

