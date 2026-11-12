#include "Menu.h"
#include "System/PadInput.h"
#include "Game.h"
#include "DxLib.h"
//#include "Singleton/SoundManager.h"

namespace
{
	constexpr int kFontSize = 30;
	constexpr int kMargin = 50;

	constexpr float kMaxScale = 1.1f;
	constexpr float kLerpT = 0.2f;

	constexpr unsigned int kTextColor = 0xffffff;

	constexpr int kStringOffset = 15;
	constexpr int kBarOffset = 20;
	constexpr int kBarHeight = 8;
}

Menu::~Menu()
{
	DeleteGraph(m_uiHandle);
	DeleteGraph(m_selectUIHandle);

	DeleteFontToHandle(m_fontHandle);
}

void Menu::Init(std::vector<Funcs> actions, int x, int y,float scale)
{
	// 関数とそのアクション名を初期化
	m_items.resize(actions.size());
	for (int i = 0; i < actions.size(); i++)
	{
		m_items[i].name = actions[i].name;
		m_items[i].action = actions[i].action;
		// scaleとcolorRateは0.0で初期化
		m_items[i].type = actions[i].type;
		m_items[i].pBarRate = actions[i].pBarRate;
		m_items[i].pSwitch = actions[i].pSwitch;
	}
	// 位置を初期化
	m_x = x;
	m_y = y;
	// 大きさを初期化
	m_scale = scale;

	m_uiHandle = LoadGraph(L"data/Graphs/Game/MenuUI.png");
	m_selectUIHandle = LoadGraph(L"data/Graphs/Game/MenuUI_Select.png");

	//m_fontHandle = CreateFontToHandle(Game::kMainFontName, kFontSize, -1);
}

void Menu::Update()
{
	auto& input = PadInput::GetInstance();
	//auto& soundManager = SoundManager::GetInstance();

	// 上下キーでメニューの選択を変更する
	if (input.IsTriggerd(XINPUT_BUTTON_DPAD_DOWN, true))
	{
		m_selectIndex++;
		if (m_selectIndex >= m_items.size())
		{
			m_selectIndex = 0;
		}
		//soundManager.PlaySoundGame(L"Select");
	}
	if (input.IsTriggerd(XINPUT_BUTTON_DPAD_UP, true))
	{
		m_selectIndex--;
		if (m_selectIndex < 0)
		{
			m_selectIndex = m_items.size() - 1;
		}
		//soundManager.PlaySoundGame(L"Select");
	}

	// 選択中の項目によって異なるアクションを実行する
	const auto& currentItem = m_items[m_selectIndex];
	switch (currentItem.type)
	{
	case Type::Normal:
		// 決定ボタンで選択中のメニューのアクションを実行する
		if (input.IsTriggerd(XINPUT_BUTTON_A))
		{
			currentItem.action();
		}
		break;
	case Type::Bar:
	case Type::Switch:
		currentItem.action();
		break;
	}

	// UIの大きさをLerpで動かす
	for (int i = 0; i < m_items.size(); i++)
	{
		// 大きさをスムーズに変える
		if (m_selectIndex == i)
		{
			m_items[i].scale = std::lerp(m_items[i].scale, kMaxScale, kLerpT);
		}
		else
		{
			m_items[i].scale = std::lerp(m_items[i].scale, 1.0f, kLerpT);
		}
		// 色をスムーズに変える
		if (m_selectIndex == i)
		{
			m_items[i].colorRate = std::lerp(m_items[i].colorRate, 1.0f, kLerpT);
		}
		else
		{
			m_items[i].colorRate = std::lerp(m_items[i].colorRate, 0.0f, kLerpT);
		}
	}
}

void Menu::Draw()
{
	for (int i = 0; i < m_items.size(); i++)
	{
		int handle = m_uiHandle;
		if (i == m_selectIndex) handle = m_selectUIHandle;
		auto actionName = m_items[i].name;

		int origBlendMode, origBlendParam;
		GetDrawBlendMode(&origBlendMode, &origBlendParam);

		// 文字の背景の描画
		int black = (1.0f - m_items[i].colorRate) * 255;
		int red = m_items[i].colorRate * 255;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, black);
		DrawRotaGraph(m_x, m_y + (kFontSize / 2 + (kFontSize + kMargin) * i) * m_scale, m_items[i].scale * m_scale, 0.0, m_uiHandle, true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, red);
		DrawRotaGraph(m_x, m_y + (kFontSize / 2 + (kFontSize + kMargin) * i) * m_scale, m_items[i].scale * m_scale, 0.0, m_selectUIHandle, true);
		SetDrawBlendMode(origBlendMode, origBlendParam);

		int graphW, graphH;
		GetGraphSize(m_uiHandle, &graphW, &graphH);
		// 文字の描画
		int strW = GetDrawExtendStringWidthToHandle(m_items[i].scale, actionName.c_str(), actionName.size(), m_fontHandle);
		if (m_items[i].type == Type::Normal)
		{
			// 文字の描画(中央揃え)
			DrawExtendStringToHandle(m_x - (strW / 2) * m_scale,
						   m_y + ((kFontSize + kMargin) * i) * m_scale,
						   m_items[i].scale * m_scale, m_items[i].scale * m_scale, actionName.c_str(), kTextColor, m_fontHandle);
		}
		else if (m_items[i].type == Type::Bar)
		{
			// 文字の描画(左揃え)
			DrawExtendStringToHandle(m_x - (graphW / 2 - kStringOffset) * m_items[i].scale * m_scale,
						   m_y + ((kFontSize + kMargin) * i) * m_scale,
						   m_items[i].scale * m_scale, m_items[i].scale * m_scale, actionName.c_str(), kTextColor, m_fontHandle);
			// バーの描画
			int x1, y1, x2, y2;
			x1 = m_x + kBarOffset * m_scale;
			y1 = m_y + (kFontSize / 2 + (kFontSize + kMargin) * i - kBarHeight / 2) * m_scale;
			x2 = m_x + (graphW / 2 - kBarOffset) * m_scale;
			y2 = m_y + (kFontSize / 2 + (kFontSize + kMargin) * i + kBarHeight / 2) * m_scale;
			DrawBox(x1, y1, x2, y2, 0x888888, true);	// バーの背景
			int barLength = abs(x2 - x1);
			x2 = x1 + barLength * *m_items[i].pBarRate;
			DrawBox(x1, y1, x2, y2, 0xff0000, true);	// バーの本体
			y1 = m_y + (kFontSize / 2 + (kFontSize + kMargin) * i) * m_scale;
			DrawCircle(x2, y1, kBarHeight, 0xffffff);
		}
		else if (m_items[i].type == Type::Switch)
		{
			// 文字の描画(左揃え)
			DrawExtendStringToHandle(m_x - (graphW / 2 - kStringOffset) * m_items[i].scale * m_scale,
						   m_y + ((kFontSize + kMargin) * i) * m_scale,
						   m_items[i].scale * m_scale, m_items[i].scale * m_scale, actionName.c_str(), kTextColor, m_fontHandle);
			// スイッチの描画
			unsigned int color = 0x888888;
			if (*m_items[i].pSwitch) color = 0xff0000;
			int x, y;
			x = m_x + (graphW / 2 - 30) * m_scale;
			y = m_y + (kFontSize / 2 + (kFontSize + kMargin) * i) * m_scale;
			DrawCircle(x, y, 10, color);
			x = x - 30;
			DrawCircle(x, y, 10, color);
			int x1, y1, x2, y2;
			x1 = x;
			y1 = y - 10;
			x2 = x + 30;
			y2 = y + 10 + 1;
			DrawBox(x1, y1, x2, y2, color, true);
			if (*m_items[i].pSwitch) x += 30;
			DrawCircle(x, y, 13, 0xffffff);
		}
	}
}