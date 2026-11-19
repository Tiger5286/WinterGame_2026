#include "EffectManager.h"
#include "Resource/ResourceManager.h"
#include "Resource/Effect.h"
#include "System/Time.h"

EffectManager& EffectManager::GetInstance()
{
	static EffectManager instance;
	return instance;
}

EffectManager::EffectManager()
{
}

EffectManager::~EffectManager()
{
	StopEffectAll();
}

void EffectManager::Update()
{
	// 再生されていないエフェクトハンドルを除外する
	std::list<int> deleteHandles;
	for (auto& handle : m_effectPlayingHandles)
	{
		if (IsEffekseer3DEffectPlaying(handle) == -1)
		{
			deleteHandles.push_back(handle);
		}
		else
		{
			SetSpeedPlayingEffekseer3DEffect(handle, Time::GetInstance().GetTimeScale());
		}
	}
	for (auto& handle : deleteHandles)
	{
		m_effectPlayingHandles.remove(handle);
	}

	UpdateEffekseer3D();
}

void EffectManager::Draw()
{
	if (m_effectPlayingHandles.empty())
	{
		return;	// 再生中のエフェクトがなければ描画しない
	}
	DrawEffekseer3D();
}

int EffectManager::PlayEffect(const std::wstring& name, const Vector3& pos)
{
	const int resourceHandle = ResourceManager::GetInstance().GetEffect(name)->GetHandle();
	// エフェクトを再生し正しく再生されたかチェック
	const int playHandle = PlayEffekseer3DEffect(resourceHandle);
	assert(playHandle != -1 && "エフェクトが正しく再生されませんでした");
	// 位置を設定
	SetPosPlayingEffekseer3DEffect(playHandle, pos.x, pos.y, pos.z);
	// リストに登録
	m_effectPlayingHandles.push_back(playHandle);
	return playHandle;
}

void EffectManager::StopEffect(int handle)
{
	StopEffekseer3DEffect(handle);
	m_effectPlayingHandles.remove(handle);
}

void EffectManager::StopEffectAll()
{
	// 再生中のエフェクトをすべて停止する
	for (auto& effect : m_effectPlayingHandles)
	{
		StopEffekseer3DEffect(effect);
		effect = -1;	// ハンドルを無効にする
	}
	// 再生中のエフェクトのリストを空にする
	m_effectPlayingHandles.clear();
}
