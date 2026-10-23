#include "SceneResult.h"
#include "DxLib.h"
#include "System/PadInput.h"
#include "SceneTitle.h"
#include "SceneManager.h"

SceneResult::SceneResult(SceneManager& sceneManager) :
	SceneBase(sceneManager)
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
#endif
}
