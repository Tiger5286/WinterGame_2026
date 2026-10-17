#include "Hitbox.h"
#include "Components/Collider/Collider.h"
#include "HitboxCollider.h"

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

void Hitbox::Update()
{
	for (auto& hitbox : m_hitboxes)
	{
		hitbox->Update();
	}
}

void Hitbox::Draw()
{
#ifdef _DEBUG
	for (auto& hitbox : m_hitboxes)
	{
		hitbox->Draw();
	}
#endif
}

void Hitbox::AddCollider(const HitboxSphere::Info& info)
{
	auto sphere = std::make_shared<HitboxSphere>();
	sphere->SetInfo(info);
	m_hitboxes.push_back(sphere);
}

void Hitbox::AddCollider(const HitboxCapsule::Info& info)
{
	auto capsule = std::make_shared<HitboxCapsule>();
	capsule->SetInfo(info);
	m_hitboxes.push_back(capsule);
}