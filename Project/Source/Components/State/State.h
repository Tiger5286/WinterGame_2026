#pragma once
#include <memory>

/// <summary>
/// ステートパターンの各ステートの親テンプレートクラス
/// </summary>
/// <typeparam name="Owner">持ち主</typeparam>
template<class Owner>
class State
{
public:
	State(Owner& owner) :
		m_owner(owner)
	{
	}
	virtual ~State() = default;

	virtual void Enter() abstract;
	virtual void Update() abstract;
	virtual void Exit() abstract;
	virtual void Draw() const {}
protected:
	Owner& m_owner;
};