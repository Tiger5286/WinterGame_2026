#include "EnemyHPUI.h"

namespace
{
	constexpr int kHPBarWidth = 200;
	constexpr int kHPBarHeight = 10;
	constexpr unsigned int kColor = 0xffff00;
	constexpr unsigned int kOutlineColor = 0x333333;
	constexpr unsigned int kOutlineThickness = 2;
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

		DrawBox(x1 - kOutlineThickness, y1 - kOutlineThickness, x2 + kOutlineThickness, y2 + kOutlineThickness, kOutlineColor, true);
		DrawBox(x1, y1, x2, y2, kColor, true);
	}
}
