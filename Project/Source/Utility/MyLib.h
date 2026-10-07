#pragma once
#include "Vector3.h"
class MyLib
{
public:
	static float GetAngleDiff(float angle1, float angle2);

	struct RayCapsuleResult
	{
		bool isHit = false;
		Vector3 hitPos;
		float dist = 0.0f;
	};

	static RayCapsuleResult CheckHitLineCapsule(const Vector3& bottom, const Vector3& top, const float radius, const Vector3& start, const Vector3& end);
};

