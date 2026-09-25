#pragma once
#include <vector>
#include <memory>
#include <cassert>

class Component;

class GameObject
{
public:
	GameObject();
	virtual ~GameObject() = default;

	// 初期化処理
	virtual void Init() abstract;
	// 更新処理
	virtual void Update() abstract;
	// 描画処理
	virtual void Draw() const abstract;

	template<class T>
	void AddComponent()
	{
		m_components.push_back(std::make_unique<T>());
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
		assert(false && "GameObject::GetComponent() : 指定のコンポーネントが取得できませんでした");
		return nullptr;
	}

protected:
	std::vector<std::unique_ptr<Component>> m_components;
};