#pragma once
#include "Components/State/State.h"

class Player;

class PlayerState : public State<Player>
{
public:
	static constexpr float kJogAccel = 0.5f;
	static constexpr float kRunAccel = 1.0f;
	static constexpr float kAirAccel = 0.5f;

	static constexpr float kMaxJogSpeed = 5.0f;
	static constexpr float kMaxRunSpeed = 10.0f;

public:
	enum class ID
	{
		Idle,
		Move,
		Jump,
		Fall,
		Land,
		
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

protected:	// ステートが使っていいプレイヤーのメンバ
	float GetCameraAngleY() const;
	void SetAngle(float angle);

	bool IsRun() const;
	void SetIsRun(bool isRun);

	// State共通で使う処理
	void UpdateMove(float accel, float maxSpeed);
};

