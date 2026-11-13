#include "SceneResult.h"
#include "DxLib.h"
#include "System/PadInput.h"
#include "SceneStageSelect.h"
#include "SceneManager.h"

SceneResult::SceneResult(SceneManager& sceneManager, bool isClear) :
	SceneBase(sceneManager),
	m_isClear(isClear)
{
}

SceneResult::~SceneResult()
{
}

void SceneResult::Init()
{
}

void SceneResult::Update()
{
	if (PadInput::GetInstance().IsTriggerd(XINPUT_BUTTON_A))
	{
		m_sceneManager.ChangeScene(std::make_shared<SceneStageSelect>(m_sceneManager),true);
		return;
	}
}

void SceneResult::Draw()
{
#ifdef _DEBUG
	DrawString(0, 0, L"SceneResult", 0xffffff);
	DrawString(0, 16, L"AボタンでSceneStageSelect", 0xffffff);
	if (m_isClear)
	{
		DrawString(0, 32, L"クリアした", 0xffffff);
	}
	else
	{
		DrawString(0, 32, L"ゲームオーバーした", 0xffffff);
	}
#endif
}
