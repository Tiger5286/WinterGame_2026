#include "DroneStateIdle.h"
#include "DroneStateShoot.h"
#include "../Drone.h"
#include "Components/State/StateMachine.h"

DroneStateIdle::DroneStateIdle(Drone& drone) :
	DroneState(drone)
{
}

DroneStateIdle::~DroneStateIdle()
{
}

void DroneStateIdle::Enter()
{
}

void DroneStateIdle::Update()
{
	// プレイヤーが射程内で直接見えたら射撃を始める
	if (CanSeePlayer())
	{
		m_owner.GetComponent<StateMachine<Drone>>()->ChangeState(std::make_unique<DroneStateShoot>(m_owner));
		return;
	}
}

void DroneStateIdle::Exit()
{
}
