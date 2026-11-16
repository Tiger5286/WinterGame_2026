#include "SceneDebug.h"
#include <vector>
#include <functional>
#include <cassert>
#include <string>
#include "SceneManager.h"
#include "SceneMain.h"
#include "SceneTitle.h"
#include "SceneResult.h"
#include "SceneStageSelect.h"
#include "ScenePause.h"
#include "SceneOption.h"
#include "System/PadInput.h"

namespace
{
	enum class Scenes
	{
		Main,
		Title,
		Result,
		StageSelect,
		Pause,
		Option,

		Num
	};

	const std::vector<std::function<void(SceneManager&)>> kSceneChangeFuncs = {
		[](SceneManager& sm) { sm.ChangeScene(std::make_shared<SceneMain>(sm),false,true); },
		[](SceneManager& sm) { sm.ChangeScene(std::make_shared<SceneTitle>(sm),false); },
		[](SceneManager& sm) { sm.ChangeScene(std::make_shared<SceneResult>(sm, true),false); },
		[](SceneManager& sm) { sm.ChangeScene(std::make_shared<SceneStageSelect>(sm),false); },
		[](SceneManager& sm) { sm.PushScene(std::make_shared<ScenePause>(sm),false); },
		[](SceneManager& sm) { sm.PushScene(std::make_shared<SceneOption>(sm),false); },
	};

	const std::vector<std::wstring> kSceneNames = {
		L"Main",
		L"Title",
		L"Result",
		L"StageSelect",
		L"Pause",
		L"Option"
	};
}

SceneDebug::SceneDebug(SceneManager& sceneManager) :
	SceneBase(sceneManager)
{
	if (static_cast<int>(Scenes::Num) != kSceneChangeFuncs.size())
	{
		assert(false);
	}
}

SceneDebug::~SceneDebug()
{
}

void SceneDebug::Init()
{
}

void SceneDebug::Update()
{
	auto& input = PadInput::GetInstance();

	if (input.IsTriggerd(XINPUT_BUTTON_DPAD_DOWN, true))
	{
		m_selectIndex++;
		if (m_selectIndex >= static_cast<int>(Scenes::Num))
		{
			m_selectIndex = 0;
		}
	}
	if (input.IsTriggerd(XINPUT_BUTTON_DPAD_UP, true))
	{
		m_selectIndex--;
		if (m_selectIndex <= -1)
		{
			m_selectIndex = static_cast<int>(Scenes::Num) - 1;
		}
	}

	if (input.IsTriggerd(XINPUT_BUTTON_A))
	{
		kSceneChangeFuncs[m_selectIndex](m_sceneManager);
	}
}

void SceneDebug::Draw()
{
	DrawString(0, 0, L"SceneDebug", 0xffffff);

	for (int i = 0; i < static_cast<int>(Scenes::Num); i++)
	{
		int x = 100;
		int y = 100;
		unsigned int color = 0xffffff;

		if (i == m_selectIndex)
		{
			color = 0x00ff00;
			x += 20;
		}
		y += i * 20;

		DrawFormatString(x, y, color, L"%s", kSceneNames[i].c_str());
	}
}
