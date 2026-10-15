#include "Hitbox.h"
#include "Components/Collider/Collider.h"

namespace
{
	constexpr unsigned int kHitboxColor = 0xffff00;
	constexpr unsigned int kWeakPointColor = 0xff0000;
}

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
#ifdef _DEBUG
	for (const auto& col : m_colliders)
	{
		unsigned int color = kHitboxColor;
		if (col.isWeakPoint)
		{
			color = kWeakPointColor;
		}

		col.pCollider->Draw(color);
	}
#endif
}