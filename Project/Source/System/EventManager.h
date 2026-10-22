#pragma once
#include <string>
#include <functional>
#include <list>
#include <memory>

class EventManager
{
public:
	static EventManager& GetInstance();
	~EventManager() = default;

	EventManager(const EventManager&) = delete;
	EventManager& operator=(const EventManager&) = delete;
private:
	EventManager() = default;

	struct EventData
	{
		int handle = -1;
		std::wstring eventName;
		std::function<void()> func;
		std::weak_ptr<void> owner;

		bool operator==(const EventData& data) const
		{
			return handle == data.handle;
		}
	};
public:

	/// <summary>
	/// 登録したイベントを発火させる。
	/// </summary>
	/// <param name="eventName">発火させるイベント名</param>
	void CallEvent(const std::wstring& eventName);

	/// <summary>
	/// イベント発火時に呼ばれる関数を登録する。
	/// </summary>
	/// <param name="eventName">イベント名</param>
	/// <param name="func">発火時に呼ばれる関数</param>
	/// <returns>管理番号</returns>
	int Register(const std::wstring& eventName, std::function<void()> func, std::weak_ptr<void> owner);

	/// <summary>
	/// 登録した関数を登録解除する。
	/// </summary>
	/// <param name="handle">登録した関数の管理番号</param>
	void UnRegister(int handle);

	/// <summary>
	/// 持っているイベントデータの持ち主が生きているか確認し、死んでいたら破棄する。
	/// </summary>
	void CheckExpired();

private:
	std::list<EventData> m_eventDatas;
	int m_newHandle = 0;
};