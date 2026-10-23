#include "SceneTitle.h"
#include "DxLib.h"
#include "System/PadInput.h"
#include "SceneManager.h"
#include "SceneMain.h"

SceneTitle::SceneTitle(SceneManager& sceneManager):
	SceneBase(sceneManager)
{
}

SceneTitle::~SceneTitle()
{
}

void SceneTitle::Init()
{
}

void SceneTitle::Update()
{
	if (PadInput::GetInstance().IsTriggerd(XINPUT_BUTTON_A))
	{
		m_sceneManager.ChangeSceneWithFade(std::make_shared<SceneMain>(m_sceneManager));
		return;
	}
}

void SceneTitle::Draw() const
{
#ifdef _DEBUG
	DrawString(0, 0, L"SceneTitle", 0xffffff);
	DrawString(0, 16, L"AボタンでSceneMainへ", 0xffffff);
#endif
}
