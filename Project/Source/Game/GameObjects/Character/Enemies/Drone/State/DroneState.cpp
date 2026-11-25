#include "DroneState.h"
#include "../Drone.h"
#include <cmath>

DroneState::DroneState(Drone& drone) :
	State(drone)
{
}

DroneState::~DroneState()
{
}

std::shared_ptr<Player> DroneState::GetPlayer() const
{
	return m_owner.m_pPlayer.lock();
}

void DroneState::Die()
{
	m_owner.m_isDead = true;
}

void DroneState::TurnToPlayerLerp(float rate)
{
	m_owner.TurnToPlayerLerp(1.0f - std::pow(1.0f - rate, m_owner.GetTimeScale()));
}

void DroneState::TurnToPlayerLimited(float maxAngle)
{
	m_owner.TurnToPlayerLimited(maxAngle * m_owner.GetTimeScale());
}

bool DroneState::CanSeePlayer() const
{
	return m_owner.CanSeePlayer();
}

Vector3 DroneState::GetMuzzlePos() const
{
	return m_owner.GetMuzzlePos();
}

Vector3 DroneState::GetMuzzleDir() const
{
	return m_owner.GetMuzzleDir();
}

Vector3 DroneState::GetLaserEnd(const Vector3& start, const Vector3& dir) const
{
	return m_owner.GetLaserEnd(start, dir);
}

void DroneState::Fire(const Vector3& start, const Vector3& dir)
{
	m_owner.Fire(start, dir);
}

void DroneState::DropSpecialNode()
{
	m_owner.DropSpecialNode();
}
