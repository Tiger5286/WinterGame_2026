#include "PlayerInfoUI.h"
#include "DxLib.h"

namespace
{
	constexpr int kLayer = 2;
}

PlayerInfoUI::PlayerInfoUI(const std::shared_ptr<Player>& pPlayer) :
	UIBase(kLayer),
	m_pPlayer(pPlayer)
{
}

PlayerInfoUI::~PlayerInfoUI()
{
}

void PlayerInfoUI::Init()
{
}

void PlayerInfoUI::Update()
{
}

void PlayerInfoUI::Draw()
{
}
