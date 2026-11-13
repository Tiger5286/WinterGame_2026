#include "SceneStageSelect.h"
#include "DxLib.h"
#include "System/PadInput.h"
#include "SceneManager.h"
#include "SceneMain.h"
#include "SceneTitle.h"

SceneStageSelect::SceneStageSelect(SceneManager& sceneManager) :
	SceneBase(sceneManager)
{
}

SceneStageSelect::~SceneStageSelect()
{
}

void SceneStageSelect::Init()
{
}

void SceneStageSelect::Update()
{
	auto& input = PadInput::GetInstance();
	if (input.IsTriggerd(XINPUT_BUTTON_A))
	{
		m_sceneManager.ChangeScene(std::make_shared<SceneMain>(m_sceneManager), true, true);
		return;
	}
	if (input.IsTriggerd(XINPUT_BUTTON_B))
	{
		m_sceneManager.ChangeScene(std::make_shared<SceneTitle>(m_sceneManager), true);
		return;
	}
}

void SceneStageSelect::Draw()
{
#ifdef _DEBUG
	DrawString(0, 0, L"SceneStageSelect", 0xffffff);
	DrawString(0, 16, L"AボタンでSceneMain BボタンでSceneTitle",0xffffff);
#endif
}
