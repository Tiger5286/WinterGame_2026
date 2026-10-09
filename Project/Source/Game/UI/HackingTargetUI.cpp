#include "HackingTargetUI.h"

namespace
{
	constexpr int kSize = 70;
	constexpr int kThickness = 10;
	constexpr unsigned int kColor = 0x0088ff;
}

HackingTargetUI::HackingTargetUI() :
	UIBase(1)
{
}

HackingTargetUI::~HackingTargetUI()
{
}

void HackingTargetUI::Init()
{
}

void HackingTargetUI::Update()
{
}

void HackingTargetUI::Draw()
{
	int x1, y1, x2, y2;
	x1 = m_info.screenPos.x - kSize;
	y1 = m_info.screenPos.y - kSize;
	x2 = m_info.screenPos.x + kSize;
	y2 = m_info.screenPos.y + kSize;
	DrawBox(x1, y1, x2, y2, kColor, false, kThickness);
}
