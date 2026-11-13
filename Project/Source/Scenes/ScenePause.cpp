#include "ScenePause.h"
#include "System/PadInput.h"
#include "SceneManager.h"
#include "Game.h"
#include "SceneTitle.h"

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

	action.name = L"オプション";
	action.action = [this]() {};
	actions.push_back(action);

	action.name = L"ステージセレクトへ戻る";
	action.action = [this]() { m_sceneManager.ResetScene(std::make_shared<SceneTitle>(m_sceneManager), true); };
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
	return;
}

void ScenePause::Draw()
{
	// 半透明の黒背景を描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	m_menu.Draw();

#ifdef _DEBUG
	DrawString(0, 0, L"ScenePause", 0xffffff);
#endif
}
