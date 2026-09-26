#pragma once
#include "PlayerState.h"

class Player;

class PlayerStateIdle : public PlayerState
{
public:
	PlayerStateIdle(Player& owner);

	void Enter() override;
	void Update() override;
	void Exit() override;

	ID GetID() const override { return ID::Idle; }
private:

};

