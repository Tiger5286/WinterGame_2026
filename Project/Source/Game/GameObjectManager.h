#pragma once
#include <list>
#include <memory>

class GameObject;

class GameObjectManager
{
public:
	GameObjectManager() = default;
	~GameObjectManager() = default;

	void Update();
	void Draw() const;

	/// <summary>
	/// オブジェクトを追加する。追加されたオブジェクトは自動的に初期化される。
	/// std::listを使用しているため、Update中にオブジェクトを追加しても安全に処理できる。
	/// </summary>
	/// <param name="gameObject"></param>
	void Add(std::shared_ptr<GameObject> gameObject);

	/// <summary>
	/// 保持しているGameObjectのうち、ハッキング可能なものを取得する。
	/// </summary>
	/// <returns></returns>
	std::list<std::weak_ptr<GameObject>> GetHackables() const;

private:
	std::list<std::shared_ptr<GameObject>> m_gameObjects;
	int m_newId = 0;
};

