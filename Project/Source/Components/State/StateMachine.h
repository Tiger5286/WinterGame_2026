#pragma once
#include <memory>
#include "State.h"
#include "Components/Component.h"

/// <summary>
/// ステートパターンの各ステートを管理するテンプレートクラス
/// </summary>
/// <typeparam name="Owner">持ち主</typeparam>
template<class Owner>
class StateMachine : public Component
{
public:
	StateMachine(Owner& owner) :
		m_owner(owner)
	{
	}

	void Update()
	{
		if (m_pCurrentState)
		{
			m_pCurrentState->Update();
		}
	}

	void Draw() const
	{
		if (m_pCurrentState)
		{
			m_pCurrentState->Draw();
		}
	}

	/// <summary>
	/// ステートを切り替える
	/// </summary>
	/// <param name="pState">変更先のステート</param>
	void ChangeState(std::unique_ptr<State<Owner>> pState)
	{
		if (m_pCurrentState)
		{
			m_pCurrentState->Exit();
		}

		m_pCurrentState = std::move(pState);

		if (m_pCurrentState)
		{
			m_pCurrentState->Enter();
		}
	}

	/// <summary>
	/// 現在のステートを取得する
	/// </summary>
	/// <typeparam name="T">Stateを継承した各オブジェクトのStateクラス</typeparam>
	/// <returns>現在のステート</returns>
	template<class T>
	T* GetState()
	{
		return dynamic_cast<T*>(m_pCurrentState.get());
	}

private:
	Owner& m_owner;

	std::unique_ptr<State<Owner>> m_pCurrentState;
};

