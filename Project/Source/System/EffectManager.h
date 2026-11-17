#pragma once
#include <string>
#include <list>
#include <map>
#include "../Utility/Vector3.h"
#include "EffekseerForDXLib.h"

class EffectManager
{
public:
	static EffectManager& GetInstance();
	virtual ~EffectManager();

	EffectManager(const EffectManager&) = delete;
	EffectManager& operator=(const EffectManager&) = delete;
private:
	EffectManager();
public:

	void Update();
	void Draw();

	/// <summary>
	/// エフェクトを再生する
	/// </summary>
	/// <param name="name">再生するエフェクトの登録名</param>
	/// <param name="pos">再生するエフェクトの位置</param>
	/// <returns>再生したエフェクトのハンドル</returns>
	int PlayEffect(const std::wstring& name, const Vector3& pos);

	/// <summary>
	/// 再生中のエフェクトを停止する
	/// </summary>
	/// <param name="handle">再生中のエフェクトのハンドル</param>
	void StopEffect(int handle);

	/// <summary>
	/// すべての再生中のエフェクトを停止する
	/// </summary>
	void StopEffectAll();

private:
	std::list<int> m_effectPlayingHandles;
};