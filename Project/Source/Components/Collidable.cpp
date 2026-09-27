#include "Collidable.h"
#include "Game/Collider/Collider.h"

void Collidable::SetCollider(std::unique_ptr<Collider> pCollider)
{
	m_pCollider = std::move(pCollider);
}