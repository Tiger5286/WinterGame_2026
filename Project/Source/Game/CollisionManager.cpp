#include "CollisionManager.h"
#include "Utility/Vector3.h"
#include "Components/Collider/Collider.h"
#include "DxLib.h"
#include "Components/Collider/CapsuleCollider.h"
#include "Components/Collider/SphereCollider.h"
#include "Components/Collider/PolygonCollider.h"
#include "Resource/Model.h"
#include "Game/GameObjects/GameObject.h"
#include "Components/Transform.h"
#include <cmath>
#include <limits>

void CollisionManager::Update()
{
	// CollisionManagerはGameObjectを所有せず、weak_ptrで参照している。
	// GameObjectが別の場所で破棄されるとweak_ptrはexpired()になるため、
	// ここで期限切れの登録を一覧から取り除く。
	for (auto it = m_pObjects.begin(); it != m_pObjects.end();)
	{
		if (it->expired())
		{
			// erase()は次の要素を指すiteratorを返すので、その値で走査を続ける。
			it = m_pObjects.erase(it);
		}
		else
		{
			// 生存している登録は残し、次の要素へ進む。
			++it;
		}
	}
}

void CollisionManager::Register(const std::shared_ptr<GameObject>& pObject)
{
	// nullのshared_ptrは登録できないため、何もせず終了する。
	if (!pObject)
	{
		return;
	}

	// 同じGameObjectの二重登録を避けるため、既存の一覧を確認する。
	for (auto it = m_pObjects.begin(); it != m_pObjects.end();)
	{
		// weak_ptrから一時的なshared_ptrを取得する。
		// lock()に失敗した場合はGameObjectがすでに破棄されている。
		if (auto registeredObject = it->lock())
		{
			// 同じGameObjectがすでに登録済みなら、追加せずに終了する。
			if (registeredObject == pObject)
			{
				return;
			}
			// 異なるGameObjectなので、次の登録を確認する。
			++it;
		}
		else
		{
			// 登録確認のついでに、期限切れのweak_ptrを掃除する。
			it = m_pObjects.erase(it);
		}
	}

	// 生存している同一オブジェクトが見つからなかったので登録する。
	// weak_ptrを保存するため、CollisionManagerがGameObjectの寿命を延ばすことはない。
	m_pObjects.push_back(pObject);
}

void CollisionManager::UnRegister(const std::shared_ptr<GameObject>& pObject)
{
	// 指定されたGameObjectの登録と、ついでに期限切れの登録を削除する。
	for (auto it = m_pObjects.begin(); it != m_pObjects.end();)
	{
		// lock()に失敗した登録はすでに無効。
		auto registeredObject = it->lock();
		if (!registeredObject || registeredObject == pObject)
		{
			// 無効な登録、または指定されたGameObjectの登録を消す。
			// erase()の戻り値を次のiteratorとして使い、要素の飛ばしを防ぐ。
			it = m_pObjects.erase(it);
		}
		else
		{
			// 削除対象ではない生存オブジェクトはそのまま残す。
			++it;
		}
	}
}

CollisionManager::HitInfo CollisionManager::CheckCollision(const Collider& movingCol, const Vector3& movedPos)
{
	HitInfo result;

	const CapsuleCollider* pCapsule = dynamic_cast<const CapsuleCollider*>(&movingCol);
	if (!pCapsule || !movingCol.IsEnable())
	{
		return result;
	}

	for (const auto& weakObj : m_pObjects)
	{
		auto obj = weakObj.lock();
		if (!obj) continue;

		Collider* other = obj->GetComponent<Collider>();

		if (other == &movingCol || !other->IsEnable()) continue;

		auto* polygon = dynamic_cast<PolygonCollider*>(other);
		if (!polygon || !polygon->GetModel()) continue;

		HitInfo polyResult;
		polyResult = ColCheckCP(*pCapsule, movedPos, *polygon);
		if (polyResult.isHit)
		{
			result.isHit = true;
			result.polyInfos.insert(result.polyInfos.end(), polyResult.polyInfos.begin(), polyResult.polyInfos.end());
		}
	}
	
	return result;
}

CollisionManager::RayInfo CollisionManager::RayCast(const Vector3& start, const Vector3& end)
{
	RayInfo result;

	// 最初のヒットを保存できるよう、最短距離の二乗を最大値で初期化する。
	float nearestDistSq = (std::numeric_limits<float>::max)();

	// 登録順ではなく距離で選ぶため、ヒットしても全モデルを調べる。
	for (const auto& weakObj : m_pObjects)
	{
		auto obj = weakObj.lock();
		if (!obj) continue;

		Collider* other = obj->GetComponent<Collider>();
		if (!other->IsEnable()) continue;

		auto* polygon = dynamic_cast<PolygonCollider*>(other);
		if (!polygon || !polygon->GetModel()) continue;

		// startからendまでの線分と、このモデルとの交差を調べる。
		const auto dxResult = MV1CollCheck_Line(polygon->GetModel()->GetHandle(), -1, start, end);
		if (!dxResult.HitFlag) continue;

		// 距離の大小だけを比較するので、平方根を求めず二乗のまま扱う。
		const Vector3 hitPos = Vector3::FromDxLib(dxResult.HitPosition);
		const float distSq = (hitPos - start).SquaredLength();
		if (distSq >= nearestDistSq) continue;

		// より近いヒットが見つかったときだけ、位置とポリゴン情報を更新する。
		nearestDistSq = distSq;
		result.isHit = true;
		result.hitPos = hitPos;
		result.polyInfo.normal = dxResult.Normal;
		result.polyInfo.pos1 = dxResult.Position[0];
		result.polyInfo.pos2 = dxResult.Position[1];
		result.polyInfo.pos3 = dxResult.Position[2];
	}
	return result;
}

CollisionManager::HitInfo CollisionManager::ColCheckCP(const CapsuleCollider& capsule, const Vector3& movedPos, const PolygonCollider& poly)
{
	// 当たり判定結果を入れる変数を準備
	HitInfo result;
	// 当たり判定をするための情報を準備
	const int modelHandle = poly.GetModel()->GetHandle();
	const Vector3 pos1 = movedPos + Vector3::Up() * capsule.GetRadius();
	const Vector3 pos2 = movedPos + Vector3::Up() * capsule.GetHeight() + Vector3::Down() * capsule.GetRadius();
	const float radius = capsule.GetRadius();
	// 当たり判定
	auto dxResult = MV1CollCheck_Capsule(modelHandle, -1, pos1, pos2, radius);
	// 準備した変数に必要な情報を代入
	for (int i = 0; i < dxResult.HitNum; i++)
	{
		result.isHit = true;
		PolyInfo info;
		info.normal = dxResult.Dim[i].Normal;
		info.pos1 = dxResult.Dim[i].Position[0];
		info.pos2 = dxResult.Dim[i].Position[1];
		info.pos3 = dxResult.Dim[i].Position[2];
		// 押し戻し量を計算
		float minDist = Segment_Triangle_MinLength(pos1, pos2, info.pos1, info.pos2, info.pos3);
		info.pushDist = radius - minDist;
		result.polyInfos.push_back(info);
	}
	// メモリを解放
	MV1CollResultPolyDimTerminate(dxResult);
	// 結果を返す
	return result;
}

CollisionManager::HitInfo CollisionManager::ColCheckCC(const CapsuleCollider& capsule1, const Vector3& movePos, const CapsuleCollider& capsule2)
{
	HitInfo result;
	if (&capsule1 == &capsule2 || !capsule1.IsEnable() || !capsule2.IsEnable())
	{
		return result;
	}

	// 現在のColliderと同じく、回転・拡大縮小しない直立カプセルとして調べる。
	// posは足元、heightは半球を含む全体の高さなので、中心線は半径分だけ内側にある。
	const float radius1 = capsule1.GetRadius();
	const float radius2 = capsule2.GetRadius();
	const Vector3& pos2 = capsule2.GetTransform().pos;
	const float bottom1 = movePos.y + radius1;
	const float top1 = movePos.y + capsule1.GetHeight() - radius1;
	const float bottom2 = pos2.y + radius2;
	const float top2 = pos2.y + capsule2.GetHeight() - radius2;

	// 中心線は両方とも縦向きなので、最短点間のXZ成分は足元同士の差と同じ。
	// 高さの範囲が重なる場合は同じ高さの点を選べるため、Y成分は0になる。
	Vector3 difference(movePos.x - pos2.x, 0.0f, movePos.z - pos2.z);
	if (bottom1 > top2)
	{
		// 自分が相手より上にある場合。
		difference.y = bottom1 - top2;
	}
	else if (top1 < bottom2)
	{
		// 自分が相手より下にある場合。
		difference.y = top1 - bottom2;
	}

	const float radiusSum = radius1 + radius2;
	const float distanceSq = difference.SquaredLength();
	// 表面が触れているだけの場合は、押し戻す必要がないので非衝突とする。
	if (distanceSq >= radiusSum * radiusSum)
	{
		return result;
	}

	const float distance = std::sqrt(distanceSq);
	result.isHit = true;
	result.contactInfo.pushDist = radiusSum - distance;
	if (distance > 0.0f)
	{
		// 相手の最短点から自分の最短点へ向かう単位ベクトルが押し出す方向。
		result.contactInfo.normal = difference / distance;
	}
	else
	{
		// 中心線が重なると方向を求められないため、移動前にいた側へ横に押し出す。
		Vector3 previousDifference = capsule1.GetTransform().pos - pos2;
		previousDifference.y = 0.0f;
		const float previousDistance = previousDifference.Length();
		if (previousDistance > 0.0f)
		{
			result.contactInfo.normal = previousDifference / previousDistance;
		}
		else
		{
			// 移動前から同じ軸上にいた場合は、右方向を代替方向にする。
			result.contactInfo.normal = Vector3::Right();
		}
	}
	return result;
}
