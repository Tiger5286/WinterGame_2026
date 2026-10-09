#pragma once
#include <vector>
#include <memory>
#include <cassert>
#include "Game/CollisionManager.h"

class Component;

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
	enum class Tag
	{
		None,
		Player,
		Enemy
	};

public:
	GameObject();
	virtual ~GameObject();

	// 初期化処理
	virtual void Init() abstract;
	// 更新処理
	virtual void Update() abstract;
	// 描画処理
	virtual void Draw() abstract;

	template<class T, class... Args>
	void AddComponent(Args&&... args)
	{
		m_components.push_back(std::make_unique<T>(std::forward<Args>(args)...));
	}

	template<class T>
	T* GetComponent()
	{
		for (const auto& component : m_components)
		{
			if (auto result = dynamic_cast<T*>(component.get()))
			{
				return result;
			}
		}
		//assert(false && "GameObject::GetComponent() : 指定のコンポーネントが取得できませんでした");
		return nullptr;
	}

	// 他オブジェクトと当たったときに呼ばれる関数
	virtual void OnCollision(GameObject& other) {};

	// プレイヤーの射撃に当たったときに呼ばれる関数
	virtual void OnWasShot(const CollisionManager::ShotInfo& info) {};

	Tag GetTag() const { return m_tag; }

	bool IsDead() const { return m_isDead; }

protected:
	std::vector<std::unique_ptr<Component>> m_components;
	Tag m_tag = Tag::None;
	bool m_isDead = false;
};