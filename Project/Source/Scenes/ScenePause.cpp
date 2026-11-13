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
	std::vector<Menu::Funcs> actions;
	Menu::Funcs action = {
		.name = L"ゲームに戻る",
		.action = [this]() { m_sceneManager.PopScene(); }
	};
	actions.push_back(action);
	m_menu.Init(actions, Game::kScreenWidth / 2, Game::kScreenHeight / 2);
}

void ScenePause::Update()
{
	auto& input = PadInput::GetInstance();
	// startボタンでポーズシーンを終わる
	if (input.IsTriggerd(XINPUT_BUTTON_START))
	{
		m_sceneManager.PopScene();
		return;
	}

	m_menu.Update();
}

void ScenePause::Draw() const
{
	// 半透明の黒背景を描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//m_menu.

#ifdef _DEBUG
	DrawString(0, 0, L"ScenePause", 0xffffff);
#endif
}
