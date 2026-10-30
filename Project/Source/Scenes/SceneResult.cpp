#include "SceneResult.h"
#include "DxLib.h"
#include "System/PadInput.h"
#include "SceneTitle.h"
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
		m_sceneManager.ChangeSceneWithFade(std::make_shared<SceneTitle>(m_sceneManager));
		return;
	}
}

void SceneResult::Draw() const
{
#ifdef _DEBUG
	DrawString(0, 0, L"SceneResult", 0xffffff);
	DrawString(0, 16, L"AボタンでSceneTitle", 0xffffff);
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
