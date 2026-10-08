#pragma once
#include <list>
#include <memory>

class UIBase;

class UIManager
{
public:
	static UIManager& GetInstance();
	~UIManager() = default;

	UIManager(const UIManager&) = delete;
	UIManager& operator=(const UIManager&) = delete;
private:
	UIManager() = default;
public:

	void Init();
	void Update();
	void Draw();

	/// <summary>
	/// UIを追加する
	/// </summary>
	/// <param name="ui">追加するUIのポインタ</param>
	void AddUI(std::shared_ptr<UIBase> ui);

private:
	std::list<std::shared_ptr<UIBase>> m_uiList;
	int m_minLayer = 0;
	int m_maxLayer = 0;
};

