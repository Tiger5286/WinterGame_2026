#include "CollisionManager.h"

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
