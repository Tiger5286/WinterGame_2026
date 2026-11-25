#include "SpecialNodeItem.h"
#include "Components/Transform.h"
#include "Character/Player/Player.h"
#include "Components/Collider/CapsuleCollider.h"
#include "System/PadInput.h"
#include "Resource/ResourceManager.h"
#include "Resource/Graph.h"

namespace
{
	// 自身の半径
	constexpr float kRadius = 50.0f;

	// 落ちてる数
	constexpr int kCount = 1;
}

SpecialNodeItem::SpecialNodeItem(const Vector3& pos, Hackable::SpecialNode type, const std::shared_ptr<Player>& pPlayer) :
	m_type(type),
	m_pPlayer(pPlayer),
	m_count(kCount)
{
	GetComponent<Transform>()->pos = pos;
}

SpecialNodeItem::~SpecialNodeItem()
{
}

void SpecialNodeItem::Init()
{
}

void SpecialNodeItem::Update()
{
	auto player = m_pPlayer.lock();
	if (!player) return;
	auto capsule = player->GetComponent<CapsuleCollider>();
	if (!capsule) return;

	const Vector3 bottom = capsule->GetBottom();
	const Vector3 top = capsule->GetTop();
	const float radius = capsule->GetRadius();

	float minLenSq = Segment_Point_MinLength_Square(bottom, top, GetComponent<Transform>()->pos);

	// 拾う距離
	if (minLenSq <= std::pow(kRadius + radius, 2))
	{
		auto& pad = PadInput::GetInstance();
		// Yで拾えるが、狙っている間(ハッキング中)は拾えない
		if (pad.IsTriggerd(XINPUT_BUTTON_Y) && !pad.IsPressedTrigger(PadInput::LR::Left))
		{
			// 拾えたら消え、拾えなかったら消えない
			if (TryPickup())
			{
				m_isDead = true;
			}
		}
#ifdef _DEBUG
		m_isCanPickupDist = true;
#endif
	}
#ifdef _DEBUG
	else
	{
		m_isCanPickupDist = false;
	}
#endif
}

void SpecialNodeItem::Draw()
{
	Vector3 pos = GetComponent<Transform>()->pos;
	const int handle = ResourceManager::GetInstance().GetGraph(L"TestSpecialNode")->GetHandle();
	DrawBillboard3D(pos + Vector3::Up() * 50.0f, 0.5f, 0.5f, 100.0f, 0.0f, handle, true);
#ifdef _DEBUG
	unsigned int color = 0xffff00;
	if (m_isCanPickupDist)
	{
		color = 0x0000ff;
	}
	DrawSphere3D(pos, kRadius, 16, color, 0xffffff, false);
#endif
}

bool SpecialNodeItem::TryPickup()
{
	auto player = m_pPlayer.lock();
	if (!player) return false;

	return player->AddSpecialNode(m_type, m_count);
}
