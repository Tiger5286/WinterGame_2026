#include "BoxTrigger.h"
#include "Character/Player/Player.h"
#include "Components/Transform.h"
#include "System/EventManager.h"

BoxTrigger::BoxTrigger(const Vector3& pos, const Vector3& size, const std::shared_ptr<Player>& pPlayer, const std::wstring& eventName) :
	m_size(size),
	m_pPlayer(pPlayer),
	m_enterEventName(eventName)
{
	GetComponent<Transform>()->pos = pos;
}

BoxTrigger::~BoxTrigger()
{
}

void BoxTrigger::Init()
{
}

void BoxTrigger::Update()
{
	const Vector3& playerPos = m_pPlayer.lock()->GetComponent<Transform>()->pos;
	const Vector3& pos = GetComponent<Transform>()->pos;

	bool isPlayerInX = playerPos.x >= pos.x && playerPos.x <= pos.x + m_size.x;
	bool isPlayerInY = playerPos.y >= pos.y && playerPos.y <= pos.y + m_size.y;
	bool isPlayerInZ = playerPos.z >= pos.z && playerPos.z <= pos.z + m_size.z;

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
	Vector3 pos = GetComponent<Transform>()->pos;

	// 底面の線
	DrawLine3D(pos, pos + Vector3(m_size.x, 0, 0), 0xff0000);
	DrawLine3D(pos, pos + Vector3(0, 0, m_size.z), 0xff0000);
	DrawLine3D(pos + Vector3(m_size.x, 0, 0), pos + Vector3(m_size.x, 0, m_size.z), 0xff0000);
	DrawLine3D(pos + Vector3(0, 0, m_size.z), pos + Vector3(m_size.x, 0, m_size.z), 0xff0000);
	// 上面の線
	DrawLine3D(pos + Vector3(0, m_size.y, 0), pos + Vector3(m_size.x, m_size.y, 0), 0xff0000);
	DrawLine3D(pos + Vector3(0, m_size.y, 0), pos + Vector3(0, m_size.y, m_size.z), 0xff0000);
	DrawLine3D(pos + Vector3(m_size.x, m_size.y, 0), pos + Vector3(m_size.x, m_size.y, m_size.z), 0xff0000);
	DrawLine3D(pos + Vector3(0, m_size.y, m_size.z), pos + Vector3(m_size.x, m_size.y, m_size.z), 0xff0000);
	// 縦の線
	DrawLine3D(pos, pos + Vector3(0, m_size.y, 0), 0xff0000);
	DrawLine3D(pos + Vector3(m_size.x, 0, 0), pos + Vector3(m_size.x, m_size.y, 0), 0xff0000);
	DrawLine3D(pos + Vector3(0, 0, m_size.z), pos + Vector3(0, m_size.y, m_size.z), 0xff0000);
	DrawLine3D(pos + Vector3(m_size.x, 0, m_size.z), pos + Vector3(m_size.x, m_size.y, m_size.z), 0xff0000);
#endif
}