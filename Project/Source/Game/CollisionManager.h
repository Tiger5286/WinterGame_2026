#pragma once
#include <list>
#include <memory>
#include <vector>
#include "Utility/Vector3.h"

class GameObject;
class Collider;
class CapsuleCollider;
class SphereCollider;
class PolygonCollider;
class Model;

class CollisionManager
{
public:
	struct PolyInfo
	{
		Vector3 normal;
		Vector3 pos1;
		Vector3 pos2;
		Vector3 pos3;
		float pushDist = 0.0f;
	};
	struct ContactInfo
	{
		Vector3 normal;
		float pushDist = 0.0f;
	};
	struct HitInfo
	{
		bool isHit = false;
		std::vector<PolyInfo> polyInfos;
		ContactInfo contactInfo;
	};
	struct RayInfo
	{
		bool isHit = false;
		Vector3 hitPos;
		PolyInfo polyInfo;
	};

public:
	CollisionManager() = default;
	~CollisionManager() = default;

	void Update();

	void Register(const std::shared_ptr<GameObject>& pObject);
	void UnRegister(const std::shared_ptr<GameObject>& pObject);

	HitInfo CheckCollision(const Collider& movingCol, const Vector3& movedPos);
	RayInfo RayCast(const Vector3& start, const Vector3& end);

private:
	HitInfo ColCheckCP(const CapsuleCollider& capsule,const Vector3& movedPos, const PolygonCollider& poly);
	HitInfo ColCheckCC(const CapsuleCollider& capsule1, const Vector3& movePos, const CapsuleCollider& capsule2);

private:
	std::list<std::weak_ptr<GameObject>> m_pObjects;
};

