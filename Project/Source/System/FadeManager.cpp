#include "FadeManager.h"
#include "DxLib.h"
#include "Game.h"

namespace
{
	constexpr int kFadeFrame = 30;
}

FadeManager& FadeManager::GetInstance()
{
	static FadeManager instance;
	return instance;
}

void FadeManager::Update()
{
	switch (m_fadeState)
	{
	case State::FadeIn :
		m_fadeRate -= 1.0f / kFadeFrame;
		if (m_fadeRate < 0.0f)
		{
			m_fadeRate = 0.0f;
			m_fadeState = State::None;
		}
		break;
	case State::FadeOut:
		m_fadeRate += 1.0f / kFadeFrame;
		if (m_fadeRate > 1.0f)
		{
			m_fadeRate = 1.0f;
			m_fadeState = State::None;
		}
		break;
	}
}

void FadeManager::Draw()
{
	int blendMode, blendParam;
	GetDrawBlendMode(&blendMode, &blendParam);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeRate * 255);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(blendMode, blendParam);
}

void FadeManager::StartFadeIn()
{
	m_fadeState = State::FadeIn;
}

void FadeManager::StartFadeOut()
{
	m_fadeState = State::FadeOut;
}

void FadeManager::SetFadeState(bool isFade)
{
	m_fadeState = State::None;

	if (isFade)
	{
		m_fadeRate = 1.0f;
	}
	else
	{	
		m_fadeRate = 0.0f;
	}
}
