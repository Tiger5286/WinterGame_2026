#include "ScenePause.h"
#include "System/PadInput.h"
#include "SceneManager.h"
#include "Game.h"

ScenePause::ScenePause(SceneManager& sceneManager) :
	SceneBase(sceneManager)
{
}

ScenePause::~ScenePause()
{
}

void ScenePause::Init()
{
}

void ScenePause::Update()
{
	auto& input = PadInput::GetInstance();
	// startボタンでポーズシーンを終わる
	if (input.IsTriggerd(XINPUT_BUTTON_START))
	{
		m_sceneManager.PopScene();
	}
}

void ScenePause::Draw() const
{
	// 半透明の黒背景を描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

#ifdef _DEBUG
	DrawString(0, 0, L"ScenePause", 0xffffff);
#endif
}
