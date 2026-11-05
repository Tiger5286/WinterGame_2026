#include "BoxTrigger.h"
#include "Character/Player/Player.h"
#include "Components/Transform.h"
#include "System/EventManager.h"
#include <algorithm>

BoxTrigger::BoxTrigger(const Vector3& pos, const Vector3& pos2, const std::shared_ptr<Player>& pPlayer, const std::wstring& eventName) :
	m_pos2(pos2),
	m_pPlayer(pPlayer),
	m_enterEventName(eventName)
{
	m_minPos = Vector3((std::min)(pos.x, m_pos2.x), (std::min)(pos.y, m_pos2.y), (std::min)(pos.z, m_pos2.z));
	m_maxPos = Vector3((std::max)(pos.x, m_pos2.x), (std::max)(pos.y, m_pos2.y), (std::max)(pos.z, m_pos2.z));
}

BoxTrigger::~BoxTrigger()
{
}

void BoxTrigger::Init()
{
}

void BoxTrigger::Update()
{
	auto player = m_pPlayer.lock();
	if (!player)
	{
		assert(false && "BoxTrigger::Update() : プレイヤーが存在しません");
		return;
	}

	const Vector3& playerPos = player->GetComponent<Transform>()->pos;
	const Vector3& pos = GetComponent<Transform>()->pos;

	bool isPlayerInX = playerPos.x >= m_minPos.x && playerPos.x <= m_maxPos.x;
	bool isPlayerInY = playerPos.y >= m_minPos.y && playerPos.y <= m_maxPos.y;
	bool isPlayerInZ = playerPos.z >= m_minPos.z && playerPos.z <= m_maxPos.z;

	if (isPlayerInX && isPlayerInY && isPlayerInZ)
	{
		if (!m_isPlayerEntered)
		{
			m_isPlayerEntered = true;
			// プレイヤーがボックスに入ったときの処理
			EventManager::GetInstance().CallEvent(m_enterEventName);
			printfDx(L"BoxTrigger : Entered\n");
		}
	}
}

void BoxTrigger::Draw()
{
#ifdef _DEBUG
	Vector3 pos = m_minPos;
	Vector3 size = m_maxPos - m_minPos;

	// 底面の線
	DrawLine3D(pos, pos + Vector3(size.x, 0, 0), 0xff0000);
	DrawLine3D(pos, pos + Vector3(0, 0, size.z), 0xff0000);
	DrawLine3D(pos + Vector3(size.x, 0, 0), pos + Vector3(size.x, 0, size.z), 0xff0000);
	DrawLine3D(pos + Vector3(0, 0, size.z), pos + Vector3(size.x, 0, size.z), 0xff0000);
	// 上面の線
	DrawLine3D(pos + Vector3(0, size.y, 0), pos + Vector3(size.x, size.y, 0), 0xff0000);
	DrawLine3D(pos + Vector3(0, size.y, 0), pos + Vector3(0, size.y, size.z), 0xff0000);
	DrawLine3D(pos + Vector3(size.x, size.y, 0), pos + Vector3(size.x, size.y, size.z), 0xff0000);
	DrawLine3D(pos + Vector3(0, size.y, size.z), pos + Vector3(size.x, size.y, size.z), 0xff0000);
	// 縦の線
	DrawLine3D(pos, pos + Vector3(0, size.y, 0), 0xff0000);
	DrawLine3D(pos + Vector3(size.x, 0, 0), pos + Vector3(size.x, size.y, 0), 0xff0000);
	DrawLine3D(pos + Vector3(0, 0, size.z), pos + Vector3(0, size.y, size.z), 0xff0000);
	DrawLine3D(pos + Vector3(size.x, 0, size.z), pos + Vector3(size.x, size.y, size.z), 0xff0000);
#endif
}
