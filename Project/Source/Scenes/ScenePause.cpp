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
	std::vector<Menu::ItemInfo> infos;
	Menu::ItemInfo info = {
		.name = L"ゲームに戻る",
		.action = [this]() { m_sceneManager.PopScene(); }
	};
	infos.push_back(info);

	info.name = L"オプション";
	info.action = [this]() {};
	infos.push_back(info);

	info.name = L"ステージセレクトへ戻る";
	info.action = [this]() { m_sceneManager.ResetScene(std::make_shared<SceneTitle>(m_sceneManager), true); };
	infos.push_back(info);

	m_menu.Init(infos, Game::kScreenWidth / 2, Game::kScreenHeight / 2);
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
