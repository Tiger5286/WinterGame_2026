#include "SceneOption.h"
#include "Utility/Vector2Int.h"
#include "Game.h"
#include "DxLib.h"
#include "System/PadInput.h"
#include "SceneManager.h"

namespace
{
	const Vector2Int kWindowLT = { Game::kScreenWidth / 4,Game::kScreenHeight / 4 };
	const Vector2Int kWindowWH = { Game::kScreenWidth / 2,Game::kScreenHeight / 2 };
	constexpr int kWindowEdgeThickness = 5;
}

SceneOption::SceneOption(SceneManager& sceneManager) :
	SceneBase(sceneManager)
{
}

SceneOption::~SceneOption()
{
}

void SceneOption::Init()
{
}

void SceneOption::Update()
{
	auto& input = PadInput::GetInstance();
	// startボタンかBボタンでポーズシーンを終わる
	if (input.IsTriggerd(XINPUT_BUTTON_START) || input.IsTriggerd(XINPUT_BUTTON_B))
	{
		m_sceneManager.PopScene();
		return;
	}
}

void SceneOption::Draw()
{
	// オプションの枠を描画
	int x1, y1, x2, y2;
	x1 = kWindowLT.x;
	y1 = kWindowLT.y;
	x2 = kWindowLT.x + kWindowWH.x;
	y2 = kWindowLT.y + kWindowWH.y;
	DrawBox(x1, y1, x2, y2, 0x000000, true);
	x1 -= kWindowEdgeThickness - 1;
	y1 -= kWindowEdgeThickness - 1;
	x2 += kWindowEdgeThickness - 1;
	y2 += kWindowEdgeThickness - 1;
	DrawBox(x1, y1, x2, y2, 0xffffff, false, kWindowEdgeThickness);

#ifdef _DEBUG
	DrawString(kWindowLT.x, kWindowLT.y, L"SceneOption", 0xffffff);
#endif
}
