#pragma once
#include "Components/State/State.h"

class Player;

class PlayerStateMove : public State<Player>
{
public:
	PlayerStateMove(Player& owner);

	void Enter() override;
	void Update() override;
	void Exit() override;

private:

};

