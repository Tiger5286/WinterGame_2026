#pragma once
#include <list>
#include <memory>
#include <vector>
#include "Utility/Vector3.h"
#include "Components/Collider/Collider.h"

class GameObject;
class CapsuleCollider;
class SphereCollider;
class PolygonCollider;
class Model;

class CollisionManager
{
public:
	struct ContactInfo
	{
		Vector3 normal;
		float pushDist = 0.0f;
		Collider::Type type;
	};
	struct HitInfo
	{
		std::vector<ContactInfo> contactInfo;
	};
	struct RayInfo
	{
		bool isHit = false;
		Vector3 hitPos;
	};

public:
	CollisionManager() = default;
	~CollisionManager() = default;

	void Update();

	/// <summary>
	/// ゲームオブジェクトを登録します。
	/// </summary>
	/// <param name="pObject">登録する GameObject を指す std::shared_ptr の const 参照。</param>
	void Register(const std::shared_ptr<GameObject>& pObject);

	/// <summary>
	/// 指定された GameObject の登録を解除します。
	/// </summary>
	/// <param name="pObject">登録を解除する GameObject を指す std::shared_ptr への const 参照。</param>
	void UnRegister(const std::shared_ptr<GameObject>& pObject);

	/// <summary>
	/// 移動するコライダーと指定位置に対して衝突判定を行い、その結果を返します。
	/// </summary>
	/// <param name="movingCol">判定対象となる移動中のコライダー（const 参照）。</param>
	/// <param name="movedPos">コライダーが移動した（または移動する）位置を表す Vector3（const 参照）。</param>
	/// <returns>衝突の有無や衝突点・法線などの情報を含む HitInfo。衝突がなければその旨を示す値を返します。</returns>
	HitInfo CheckCollision(const Collider& movingCol, const Vector3& movedPos);

	/// <summary>
	/// 指定した始点から終点に向けてレイを投射し、その結果を取得します。(ポリゴンとしか当たらない)
	/// </summary>
	/// <param name="start">レイの始点となる3次元座標。</param>
	/// <param name="end">レイの終点となる3次元座標（投射方向と長さを定義）。</param>
	/// <returns>レイキャストの結果を表すRayInfoオブジェクト。ヒットの有無や、衝突が発生した場合は衝突位置、法線、距離などの情報を含みます。</returns>
	RayInfo RayCast(const Vector3& start, const Vector3& end);

	void CheckCollShot(const Vector3& start, const Vector3& end);

private:
	HitInfo ColCheckCP(const CapsuleCollider& capsule,const Vector3& movedPos, const PolygonCollider& poly);
	HitInfo ColCheckCC(const CapsuleCollider& capsule1, const Vector3& movePos, const CapsuleCollider& capsule2);

private:
	std::list<std::weak_ptr<GameObject>> m_pObjects;
};

