#include "EnemyHPUI.h"

namespace
{
	constexpr int kHPBarWidth = 200;
	constexpr int kHPBarHeight = 10;
}

EnemyHPUI::EnemyHPUI() :
	UIBase(0)
{
}

EnemyHPUI::~EnemyHPUI()
{
}

void EnemyHPUI::Init()
{
}

void EnemyHPUI::Update()
{
}

void EnemyHPUI::Draw()
{
	if (!CheckCameraViewClip(m_info.uiPos))
	{
		Vector3 screenPos = Vector3::FromDxLib(ConvWorldPosToScreenPos(m_info.uiPos));

		float rate = 1.0f - static_cast<float>(m_info.nowHP) / m_info.maxHP;

		int x1, y1, x2, y2;
		x1 = screenPos.x - kHPBarWidth / 2;
		y1 = screenPos.y - kHPBarHeight / 2;
		x2 = screenPos.x + kHPBarWidth / 2 - kHPBarWidth * rate;
		y2 = screenPos.y + kHPBarHeight / 2;

		DrawBox(x1, y1, x2, y2, 0xffff00, true);
	}
}
