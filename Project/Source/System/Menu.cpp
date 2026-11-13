#include "Menu.h"
#include "System/PadInput.h"
#include "Game.h"
#include "DxLib.h"
//#include "Singleton/SoundManager.h"
#include "Resource/ResourceManager.h"
#include "Resource/Graph.h"

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

	constexpr int kWidth = 350;
	constexpr int kHeight = 60;
	constexpr int kEdgeThickness = 6;
}

Menu::Menu()
{
}

Menu::~Menu()
{
}

void Menu::Init(std::vector<ItemInfo> infos, int x, int y,float scale)
{
	// 関数とそのアクション名を初期化
	m_items.resize(infos.size());
	for (int i = 0; i < infos.size(); i++)
	{
		m_items[i].info = infos[i];
		// scaleとcolorRateは0.0で初期化
	}
	// 位置を初期化
	m_x = x;
	m_y = y;
	// 大きさを初期化
	m_scale = scale;
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
	switch (currentItem.info.type)
	{
	case Type::Normal:
		// 決定ボタンで選択中のメニューのアクションを実行する
		if (input.IsTriggerd(XINPUT_BUTTON_A))
		{
			currentItem.info.action();
		}
		break;
	case Type::Bar:
	case Type::Switch:
		currentItem.info.action();
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
	auto& resourceManager = ResourceManager::GetInstance();
	for (int i = 0; i < m_items.size(); i++)
	{
		int fontHandle = resourceManager.GetFont(L"MainFont30")->GetHandle();
		int graphHandle = resourceManager.GetGraph(L"Menu")->GetHandle();
		int selectGraphHandle = resourceManager.GetGraph(L"MenuSelect")->GetHandle();
		auto actionName = m_items[i].info.name;

		int origBlendMode, origBlendParam;
		GetDrawBlendMode(&origBlendMode, &origBlendParam);

		// 文字の背景の描画
		int black = (1.0f - m_items[i].colorRate) * 255;
		int red = m_items[i].colorRate * 255;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, black);
		DrawRotaGraph(m_x, m_y + (kFontSize / 2 + (kFontSize + kMargin) * i) * m_scale, m_items[i].scale * m_scale, 0.0, graphHandle, true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, red);
		DrawRotaGraph(m_x, m_y + (kFontSize / 2 + (kFontSize + kMargin) * i) * m_scale, m_items[i].scale * m_scale, 0.0, selectGraphHandle, true);
		SetDrawBlendMode(origBlendMode, origBlendParam);

		int graphW, graphH;
		GetGraphSize(graphHandle, &graphW, &graphH);
		// 文字の描画
		int strW = GetDrawExtendStringWidthToHandle(m_items[i].scale, actionName.c_str(), actionName.size(), fontHandle);
		if (m_items[i].info.type == Type::Normal)
		{
			// 文字の描画(中央揃え)
			DrawExtendStringToHandle(m_x - (strW / 2) * m_scale,
						   m_y + ((kFontSize + kMargin) * i) * m_scale,
						   m_items[i].scale * m_scale, m_items[i].scale * m_scale, actionName.c_str(), kTextColor, fontHandle);
		}
		else if (m_items[i].info.type == Type::Bar)
		{
			// 文字の描画(左揃え)
			DrawExtendStringToHandle(m_x - (graphW / 2 - kStringOffset) * m_items[i].scale * m_scale,
						   m_y + ((kFontSize + kMargin) * i) * m_scale,
						   m_items[i].scale * m_scale, m_items[i].scale * m_scale, actionName.c_str(), kTextColor, fontHandle);
			// バーの描画
			int x1, y1, x2, y2;
			x1 = m_x + kBarOffset * m_scale;
			y1 = m_y + (kFontSize / 2 + (kFontSize + kMargin) * i - kBarHeight / 2) * m_scale;
			x2 = m_x + (graphW / 2 - kBarOffset) * m_scale;
			y2 = m_y + (kFontSize / 2 + (kFontSize + kMargin) * i + kBarHeight / 2) * m_scale;
			DrawBox(x1, y1, x2, y2, 0x888888, true);	// バーの背景
			int barLength = abs(x2 - x1);
			x2 = x1 + barLength * *m_items[i].info.pBarRate;
			DrawBox(x1, y1, x2, y2, 0xff0000, true);	// バーの本体
			y1 = m_y + (kFontSize / 2 + (kFontSize + kMargin) * i) * m_scale;
			DrawCircle(x2, y1, kBarHeight, 0xffffff);
		}
		else if (m_items[i].info.type == Type::Switch)
		{
			// 文字の描画(左揃え)
			DrawExtendStringToHandle(m_x - (graphW / 2 - kStringOffset) * m_items[i].scale * m_scale,
						   m_y + ((kFontSize + kMargin) * i) * m_scale,
						   m_items[i].scale * m_scale, m_items[i].scale * m_scale, actionName.c_str(), kTextColor, fontHandle);
			// スイッチの描画
			unsigned int color = 0x888888;
			if (*m_items[i].info.pSwitch) color = 0xff0000;
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
			if (*m_items[i].info.pSwitch) x += 30;
			DrawCircle(x, y, 13, 0xffffff);
		}
	}
}