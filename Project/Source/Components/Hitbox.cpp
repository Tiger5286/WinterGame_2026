#include "Hitbox.h"
#include "Components/Collider/Collider.h"

Hitbox::Hitbox()
{
}

Hitbox::~Hitbox()
{
}

void Hitbox::Init(std::vector<Info> colDatas)
{
	m_colliders = colDatas;
}

void Hitbox::Draw()
{
	for (const auto& col : m_colliders)
	{
		col.pCollider->Draw();
	}
}