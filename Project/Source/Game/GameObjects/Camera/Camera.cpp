#include "Camera.h"

namespace
{
	const Vector3 kDefaultPos = { 0,100,-700 };
}

void Camera::Init()
{
	m_transform.pos = kDefaultPos;
	SetCameraPositionAndTarget_UpVecY(m_transform.pos, m_targetPos);
}

void Camera::Update()
{
	SetCameraPositionAndTarget_UpVecY(m_transform.pos, m_targetPos);
}

void Camera::Draw() const
{
	// èàóùÇ»Çµ
}