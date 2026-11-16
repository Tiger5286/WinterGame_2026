#include "SceneStageSelect.h"
#include "DxLib.h"
#include "System/PadInput.h"
#include "SceneManager.h"
#include "SceneMain.h"
#include "SceneTitle.h"

namespace
{
	constexpr int kStageNum = 2;
}

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

	if (input.IsTriggerd(XINPUT_BUTTON_DPAD_RIGHT, true))
	{
		m_selectIndex++;
		if (m_selectIndex >= kStageNum)
		{
			m_selectIndex = kStageNum - 1;
		}
	}
	if (input.IsTriggerd(XINPUT_BUTTON_DPAD_LEFT, true))
	{
		m_selectIndex--;
		if (m_selectIndex <= -1)
		{
			m_selectIndex = 0;
		}
	}

	if (input.IsTriggerd(XINPUT_BUTTON_A))
	{
		m_sceneManager.ChangeScene(std::make_shared<SceneMain>(m_sceneManager, m_selectIndex + 1), true, true);
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

	for (int i = 0; i < kStageNum; i++)
	{
		int x = 100;
		int y = 100 + i * 20;
		unsigned int color = 0xffffff;
		if (m_selectIndex == i)
		{
			x += 20;
			color = 0x00ff00;
		}
		std::wstring str = L"stage" + std::to_wstring(i + 1);
		DrawFormatString(x, y, color, L"%s", str.c_str());
	}
#endif
}
