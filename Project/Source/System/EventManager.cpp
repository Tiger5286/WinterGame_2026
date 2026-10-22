#include "EventManager.h"

EventManager& EventManager::GetInstance()
{
	static EventManager instance;
	return instance;
}

void EventManager::CallEvent(const std::wstring& eventName)
{
	// 関数を呼ぶ前に関数の持ち主が生きているかどうかチェック
	CheckExpired();

	// 対応した名前の関数を呼ぶ
	for (auto& data : m_eventDatas)
	{
		if (data.eventName == eventName)
		{
			data.func();
		}
	}
}

int EventManager::Register(const std::wstring& eventName, std::function<void()> func, std::weak_ptr<void> owner)
{
	// 処理の前に関数の持ち主が生きているかどうかチェック
	CheckExpired();

	// 管理番号を更新
	m_newHandle++;
	// 登録するイベントデータを作成
	EventData newData = {
		.handle = m_newHandle,
		.eventName = eventName,
		.func = func,
		.owner = owner
	};
	// 登録
	m_eventDatas.push_back(newData);
	// 新しいイベントのハンドルを返す
	return newData.handle;
}

void EventManager::UnRegister(int handle)
{
	// 処理の前に関数の持ち主が生きているかどうかチェック
	CheckExpired();

	// ハンドルが一致するイベントを削除
	for (auto& data : m_eventDatas)
	{
		if (data.handle == handle)
		{
			m_eventDatas.remove(data);
			break;
		}
	}
}

void EventManager::CheckExpired()
{
	// イベント関数の持ち主が生きているかどうかチェックし、死んでいたら登録解除
	std::list<EventData> deleteDatas;
	for (auto& data : m_eventDatas)
	{
		std::shared_ptr<void> shared = data.owner.lock();
		if (shared == nullptr)
		{
			deleteDatas.push_back(data);
		}
	}
	for (auto& data : deleteDatas)
	{
		UnRegister(data.handle);
	}
}
