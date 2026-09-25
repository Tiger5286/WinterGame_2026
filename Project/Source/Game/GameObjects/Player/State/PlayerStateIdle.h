#pragma once
#include "Components/State/State.h"

class Player;

class PlayerStateIdle : public State<Player>
{
public:
	PlayerStateIdle(Player& owner);

	void Enter() override;
	void Update() override;
	void Exit() override;

private:

};

