#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Hackable.h"

GameObject::GameObject()
{
	AddComponent<Transform>();
}

GameObject::~GameObject()
{
}

bool GameObject::IsHackable()
{
	auto hackable = GetComponent<Hackable>();
	// 無効化されているものはハッキング対象にしない
	return hackable && hackable->IsEnable();
}
