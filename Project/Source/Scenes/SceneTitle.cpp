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
		m_sceneManager.ChangeScene(std::make_shared<SceneMain>(m_sceneManager), true);
		return;
	}
}

void SceneTitle::Draw() const
{
	DrawBox(100, 100, 200, 200, 0xffffff, true);
#ifdef _DEBUG
	DrawString(0, 0, L"SceneTitle", 0xffffff);
	DrawString(0, 16, L"AボタンでSceneMainへ", 0xffffff);
#endif
}
