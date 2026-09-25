#include "GameObject.h"
#include "Components/Transform.h"

GameObject::GameObject()
{
	AddComponent<Transform>();
}
