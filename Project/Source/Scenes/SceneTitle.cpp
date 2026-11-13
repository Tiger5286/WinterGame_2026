#include "SceneTitle.h"
#include "DxLib.h"
#include "System/PadInput.h"
#include "SceneManager.h"
#include "SceneMain.h"
#include "Resource/ResourceManager.h"
#include "Resource/Font.h"

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
		m_sceneManager.ChangeScene(std::make_shared<SceneMain>(m_sceneManager), true,true);
		return;
	}
}

void SceneTitle::Draw()
{
	// テスト用四角
	DrawBox(100, 100, 200, 200, 0xffffff, true);

	// テスト用文字
	Font* font = ResourceManager::GetInstance().GetFont(L"MainFont30");
	DrawStringToHandle(100, 300, L"フォントテスト用文字列", 0xffffff, font->GetHandle());
#ifdef _DEBUG
	DrawString(0, 0, L"SceneTitle", 0xffffff);
	DrawString(0, 16, L"AボタンでSceneMainへ", 0xffffff);
#endif
}
