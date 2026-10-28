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
#include "Components/Hitbox/Hitbox.h"
#include "Components/Hitbox/HitboxCollider.h"
#include "Components/Hitbox/HitboxSphere.h"
#include "Components/Hitbox/HitboxCapsule.h"
#include "Utility/MyLib.h"

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
	// OnCollision()を呼ぶために全てのオブジェクトで当たり判定を行う
	for (auto it1 = m_pObjects.begin(); it1 != m_pObjects.end(); it1++)
	{
		for (auto it2 = std::next(it1); it2!=m_pObjects.end();it2++)
		{
			auto obj1 = it1->lock();
			auto obj2 = it2->lock();
			// オブジェクトがnullなら次へ
			if (!obj1 || !obj2) continue;
			// 同じオブジェクトなら次へ
			if (obj1 == obj2) continue;
			// コライダーがnullなら次へ
			auto obj1Col = obj1->GetComponent<Collider>();
			auto obj2Col = obj2->GetComponent<Collider>();
			if (!obj1Col || !obj2Col) continue;
			// コライダーの種類を特定
			auto obj1sphere = obj1->GetComponent<SphereCollider>();
			auto obj2sphere = obj2->GetComponent<SphereCollider>();
			auto obj1capsule = obj1->GetComponent<CapsuleCollider>();
			auto obj2capsule = obj2->GetComponent<CapsuleCollider>();
			auto obj1polygon = obj1->GetComponent<PolygonCollider>();
			auto obj2polygon = obj2->GetComponent<PolygonCollider>();
			// どちらもポリゴンなら次へ
			if (obj1polygon && obj2polygon) continue;
			// 当たり判定
			bool isHit = false;
			if (obj1sphere)
			{
				if (obj2sphere)
				{
					if (ColCheckSS(*obj1sphere, *obj2sphere)) isHit = true;
				}
				else if (obj2capsule)
				{
					if (ColCheckSC(*obj1sphere, *obj2capsule)) isHit = true;
				}
				else if (obj2polygon)
				{
					if (ColCheckSP(*obj1sphere, *obj2polygon)) isHit = true;
				}
			}
			else if (obj1capsule)
			{
				if (obj2sphere)
				{
					if (ColCheckSC(*obj2sphere, *obj1capsule)) isHit = true;
				}
				else if (obj2capsule)
				{
					if (ColCheckCC(*obj1capsule, *obj2capsule)) isHit = true;
				}
				else if (obj2polygon)
				{
					if (ColCheckCP(*obj1capsule, *obj2polygon)) isHit = true;
				}
			}
			else if (obj1polygon)
			{
				if (obj2sphere)
				{
					if (ColCheckSP(*obj2sphere, *obj1polygon)) isHit = true;
				}
				else if (obj2capsule)
				{
					if (ColCheckCP(*obj2capsule, *obj1polygon)) isHit = true;
				}
			}
			// 当たっていたらお互いのOnCollision()を呼ぶ
			if (isHit)
			{
				obj1->OnCollision(*obj2);
				obj2->OnCollision(*obj1);
			}
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

	// コライダーが有効でないならreturn
	if (!movingCol.IsEnable()) return result;

	// コライダーがカプセルで無ければreturn
	// これはあとで全部対応するように直す
	const CapsuleCollider* pCapsule = dynamic_cast<const CapsuleCollider*>(&movingCol);
	if (!pCapsule) return result;

	// 他の当たり判定するオブジェクトを回す
	for (const auto& weakObj : m_pObjects)
	{
		// オブジェクトがnullなら次へ
		auto obj = weakObj.lock();
		if (!obj) continue;

		// オブジェクトのコライダーを取得
		Collider* other = obj->GetComponent<Collider>();

		// 自分と同じ、もしくは有効でないなら次へ
		if (other == &movingCol || !other->IsEnable()) continue;

		// 当たり判定結果を入れる用の変数を用意
		HitInfo info;

		// 当たり判定の種類を取得
		auto* polygon = dynamic_cast<PolygonCollider*>(other);
		auto* capsule = dynamic_cast<CapsuleCollider*>(other);
		auto* sphere = dynamic_cast<SphereCollider*>(other);
		// ポリゴンなら当たり判定
		if (polygon && polygon->GetModel())	// モデルが無い場合スルー
		{
			info = ColCheckCP(*pCapsule, movedPos, *polygon);
			result.contactInfo.insert(result.contactInfo.end(), info.contactInfo.begin(), info.contactInfo.end());
		}
		else if (capsule) // カプセルなら当たり判定
		{
			info = ColCheckCC(*pCapsule, movedPos, *capsule);
			result.contactInfo.insert(result.contactInfo.end(), info.contactInfo.begin(), info.contactInfo.end());
		}
	}
	
	return result;
}

CollisionManager::RayInfo CollisionManager::RayCast(const Vector3& start, const Vector3& end, bool isPolygonOnly)
{
	RayInfo result;

	// 最初のヒットを保存できるよう、最短距離の二乗を最大値で初期化する。
	float nearestDistSq = FLT_MAX;

	// 登録順ではなく距離で選ぶため、ヒットしても全モデルを調べる。
	for (const auto& weakObj : m_pObjects)
	{
		// オブジェクトがnullなら次へ
		auto obj = weakObj.lock();
		if (!obj) continue;
		// コライダーが有効でないなら次へ
		Collider* other = obj->GetComponent<Collider>();
		if (!other->IsEnable()) continue;
		// ポリゴンの当たり判定
		auto* polygon = dynamic_cast<PolygonCollider*>(other);
		if (polygon && polygon->GetModel())
		{
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
		}
		// ポリゴンのみ判定する場合はカプセルを無視する
		if (isPolygonOnly) continue;
		// カプセルの当たり判定
		auto* capsule = dynamic_cast<CapsuleCollider*>(other);
		if (capsule)
		{
			const Vector3 bottom = capsule->GetTransform().pos + Vector3::Up() * capsule->GetRadius();
			const Vector3 top = capsule->GetTransform().pos + Vector3::Up() * capsule->GetHeight() + Vector3::Down() * capsule->GetRadius();
			const float radius = capsule->GetRadius();
			auto hitResult = MyLib::CheckHitLineCapsule(bottom, top, radius, start, end);
			if (!hitResult.isHit) continue;
			const float distSq = (hitResult.hitPos - start).SquaredLength();
			if (distSq >= nearestDistSq) continue;
			nearestDistSq = distSq;
			result.isHit = true;
			result.hitPos = hitResult.hitPos;
		}
	}
	return result;
}

void CollisionManager::CheckCollShot(const Vector3& start, const Vector3& end, ShotInfo& shotInfo)
{
	float nearestDistSq = FLT_MAX;

	GameObject* hitObj = nullptr;

	for (auto& weakObj : m_pObjects)
	{
		auto obj = weakObj.lock();
		if (obj == nullptr) continue;
		auto polygon = obj->GetComponent<PolygonCollider>();
		if (polygon)
		{
			const auto dxResult = MV1CollCheck_Line(polygon->GetModel()->GetHandle(), -1, start, end);
			if (!dxResult.HitFlag) continue;
			const Vector3 hitPos = Vector3::FromDxLib(dxResult.HitPosition);
			const float distSq = (hitPos - start).SquaredLength();
			if (distSq < nearestDistSq)
			{
				shotInfo.isWeakPoint = false;
				hitObj = obj.get();
				nearestDistSq = distSq;
			}
		}
		else
		{
			auto hitBox = obj->GetComponent<Hitbox>();
			if (!hitBox) continue;

			for (auto& col : hitBox->GetHitboxes())
			{
				if (col->GetType() == HitboxCollider::Type::Sphere)
				{
					auto sphere = std::dynamic_pointer_cast<HitboxSphere>(col);
					auto result = MyLib::CheckHitLineSphere(sphere->GetPos(), sphere->GetInfo().radius, start, end);
					if (result.isHit)
					{
						float sqLen = (result.hitPos - start).SquaredLength();
						if (sqLen < nearestDistSq)
						{
							hitObj = obj.get();
							nearestDistSq = sqLen;
							shotInfo.isWeakPoint = sphere->GetInfo().isWeakPoint;
						}
					}
				}
				else if (col->GetType() == HitboxCollider::Type::Capsule)
				{
					auto capsule = std::dynamic_pointer_cast<HitboxCapsule>(col);
					auto result = MyLib::CheckHitLineCapsule(capsule->GetBottom(), capsule->GetTop(), capsule->GetInfo().radius, start, end);
					if (result.isHit)
					{
						float sqLen = (result.hitPos - start).SquaredLength();
						if (sqLen < nearestDistSq)
						{
							hitObj = obj.get();
							nearestDistSq = sqLen;
							shotInfo.isWeakPoint = capsule->GetInfo().isWeakPoint;
						}
					}
				}
			}
		}
	}

	if (hitObj)
	{
		hitObj->OnWasShot(shotInfo);
	}
	else
	{
		printfDx(L"何にも当たらなかった\n");
	}
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
		ContactInfo info;
		info.type = Collider::Type::Polygon;
		info.normal = dxResult.Dim[i].Normal;
		// 押し戻し量を計算
		float minDist = Segment_Triangle_MinLength(pos1, pos2,
			dxResult.Dim[i].Position[0], dxResult.Dim[i].Position[1], dxResult.Dim[i].Position[2]);
		info.pushDist = radius - minDist;
		result.contactInfo.push_back(info);
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

	const Vector3 pos1 = movePos;
	const Vector3 pos2 = capsule2.GetTransform().pos;

	const Vector3 bottom1 = pos1 + Vector3::Up() * capsule1.GetRadius();
	const Vector3 top1 = pos1 + Vector3::Up() * capsule1.GetHeight() + Vector3::Down() * capsule1.GetRadius();
	const float radius1 = capsule1.GetRadius();
	const Vector3 bottom2 = pos2 + Vector3::Up() * capsule2.GetRadius();
	const Vector3 top2 = pos2 + Vector3::Up() * capsule2.GetHeight() + Vector3::Down() * capsule2.GetRadius();
	const float radius2 = capsule2.GetRadius();

	// カプセル同士のXZ平面での距離を計算
	Vector3 difference = Vector3(pos1.x - pos2.x, 0.0f, pos1.z - pos2.z);

	// 高さが離れている場合は近い端点同士の距離を計算する
	if (bottom1.y > top2.y)
	{
		difference.y = bottom1.y - top2.y;
	}
	else if (bottom2.y > top1.y)
	{
		difference.y = top1.y - bottom2.y;
	}

	// differenceはカプセル同士の最短距離ベクトルを表す(2->1の方向)
	float distance = difference.Length();

	// 当たり判定
	if (distance < radius1 + radius2)
	{
		// 当たっている場合、押し戻し量と法線を計算する
		ContactInfo info;
		info.type = Collider::Type::Capsule;
		info.normal = difference.Normalized();
		info.pushDist = radius1 + radius2 - distance;
		result.contactInfo.push_back(info);
	}

	return result;
}

bool CollisionManager::ColCheckSS(const SphereCollider& sphere1, const SphereCollider& sphere2)
{
	float sqLen = (sphere1.GetTransform().pos - sphere2.GetTransform().pos).SquaredLength();
	float hitLen = sphere1.GetRadius() + sphere2.GetRadius();
	if (sqLen < hitLen * hitLen)
	{
		return true;
	}
	return false;
}

bool CollisionManager::ColCheckSC(const SphereCollider& sphere, const CapsuleCollider& capsule)
{
	Vector3 bottom = capsule.GetTransform().pos + Vector3::Up() * capsule.GetRadius();
	Vector3 top = capsule.GetTransform().pos + Vector3::Up() * capsule.GetHeight() + Vector3::Down() * capsule.GetRadius();
	float sqLen = Segment_Point_MinLength_Square(bottom, top, sphere.GetTransform().pos);
	float hitLen = sphere.GetRadius() + capsule.GetRadius();
	if (sqLen < hitLen * hitLen)
	{
		return true;
	}
	return false;
}

bool CollisionManager::ColCheckSP(const SphereCollider& sphere, const PolygonCollider& polygon)
{
	bool isHit = false;
	auto dxResult = MV1CollCheck_Sphere(polygon.GetModel()->GetHandle(), -1, sphere.GetTransform().pos, sphere.GetRadius());
	isHit = dxResult.HitNum;
	MV1CollResultPolyDimTerminate(dxResult);
	return isHit;
}

bool CollisionManager::ColCheckCC(const CapsuleCollider& capsule1, const CapsuleCollider& capsule2)
{
	float sqLen = Segment_Segment_MinLength_Square(capsule1.GetBottom(), capsule1.GetTop(), capsule2.GetBottom(), capsule2.GetTop());
	float hitLen = capsule1.GetRadius() + capsule2.GetRadius();
	if (sqLen < hitLen * hitLen)
	{
		return true;
	}
	return false;
}

bool CollisionManager::ColCheckCP(const CapsuleCollider& capsule, const PolygonCollider& polygon)
{
	bool isHit = false;
	auto dxResult = MV1CollCheck_Capsule(polygon.GetModel()->GetHandle(), -1, capsule.GetBottom(), capsule.GetTop(), capsule.GetRadius());
	isHit = dxResult.HitNum;
	MV1CollResultPolyDimTerminate(dxResult);
	return isHit;
}