#include "CollisionManager.h"
#include "Utility/Vector3.h"
#include "Components/Collider/Collider.h"
#include "DxLib.h"
#include "Components/Collider/CapsuleCollider.h"
#include "Components/Collider/SphereCollider.h"
#include "Components/Collider/PolygonCollider.h"
#include "Resource/Model.h"
#include "Game/GameObjects/GameObject.h"

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

		result = ColCheckCP(*pCapsule, movedPos, *polygon);
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
		result.polyInfos.push_back(info);
	}
	// メモリを解放
	MV1CollResultPolyDimTerminate(dxResult);
	// 結果を返す
	return result;
}