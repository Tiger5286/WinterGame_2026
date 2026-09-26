#pragma once
#include "Components/State/State.h"

class Player;

class PlayerState : public State<Player>
{
public:
	enum class ID
	{
		Idle,
		Move,
		
		Num
	};

public:
	PlayerState(Player& player) :
		State(player)
	{
	}
	virtual ~PlayerState() = default;

	virtual void Enter() override abstract;
	virtual void Update() override abstract;
	virtual void Exit() override abstract;

	virtual ID GetID() const abstract;
};

