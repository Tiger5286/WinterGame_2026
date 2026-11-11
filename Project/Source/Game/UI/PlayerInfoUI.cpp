#include "PlayerInfoUI.h"
#include "DxLib.h"
#include "Utility/Vector2Int.h"
#include "Game/GameObjects/Character/Player/Player.h"
#include "Game.h"

namespace
{
	constexpr int kLayer = 2;

	const Vector2Int kHPBarLT = { 20,20 };
	const Vector2Int kHPBarWH = { 500,40 };
	constexpr float kHPBarFrameThickness = 4;

	const Vector2Int kThrusterPos = { Game::kScreenWidth / 2 ,Game::kScreenHeight - 50 };
	const Vector2Int kThrusterWH = { 200,20 };
	constexpr float kThrusterFrameThickness = 3;

	const Vector2Int kAmmoUILT = { Game::kScreenWidth - 100,Game::kScreenHeight - 100 };
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
	auto player = m_pPlayer.lock();
	if (!player) return;

	// HPバー描画
	float rate = static_cast<float>(player->GetHP()) / static_cast<float>(player->GetMaxHP());
	int x1, y1, x2, y2;
	x1 = kHPBarLT.x;
	y1 = kHPBarLT.y;
	x2 = kHPBarLT.x + (kHPBarWH.x * rate);
	y2 = kHPBarLT.y + kHPBarWH.y;
	DrawBox(x1, y1, x2, y2, 0xffff00, true);
	x2 = kHPBarLT.x + kHPBarWH.x;
	DrawBox(x1, y1, x2, y2, 0x888888, false, kHPBarFrameThickness);

	// スラスター描画
	rate = static_cast<float>(player->GetThruster()) / static_cast<float>(player->GetMaxThruster());
	x1 = kThrusterPos.x;
	y1 = kThrusterPos.y;
	x2 = kThrusterPos.x + (kThrusterWH.x * rate);
	y2 = kThrusterPos.y + kThrusterWH.y;
	DrawBox(x1, y1, x2, y2, 0xffffff, true);
	x2 = kThrusterPos.x - (kThrusterWH.x * rate);
	DrawBox(x1, y1, x2, y2, 0xffffff, true);
	x1 = kThrusterPos.x - kThrusterWH.x;
	x2 = kThrusterPos.x + kThrusterWH.x;
	DrawBox(x1, y1, x2, y2, 0x888888, false, kThrusterFrameThickness);

	// 残弾数描画
	const wchar_t* ammoStr = L"%d / %d";
	int width = GetDrawFormatStringWidth(ammoStr, player->GetAmmo(), player->GetMaxAmmo());
	x1 = kAmmoUILT.x - 3;
	y1 = kAmmoUILT.y - 3;
	x2 = kAmmoUILT.x + width + 3;
	y2 = kAmmoUILT.y + 16 + 3;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(x1, y1, x2, y2, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawFormatString(kAmmoUILT.x, kAmmoUILT.y, 0xffffff, ammoStr, player->GetAmmo(), player->GetMaxAmmo());
}
