#pragma once
#include "PlayerState.h"

class PlayerStateMove : public PlayerState
{
public:
	static constexpr float kMaxJogSpeed = 5.0f;
	static constexpr float kMaxRunSpeed = 10.0f;

public:
	PlayerStateMove(Player& owner);

	void Enter() override;
	void Update() override;
	void Exit() override;

	ID GetID() const override { return ID::Move; }

	bool IsRun() const { return m_isRun; }
private:
	bool m_isRun = false;
};

