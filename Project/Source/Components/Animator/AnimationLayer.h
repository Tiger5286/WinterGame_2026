#pragma once
#include <vector>
#include <string>
#include "DxLib.h"

class Animation;
class Model;

class AnimationLayer
{
public:
	struct PlayBack
	{
		Animation* pAnimation = nullptr;
		int attachIndex = -1;
		float time = 0.0f;
	};

public:
	void Init(Model* pModel, const std::vector<std::wstring>& boneNames, const std::vector<std::wstring>& exclusionBoneNames);

	void Play(Animation* pAnimation,float startTime = 0.0f);
	void Stop();
	void Apply(float layerWeight);
	void Update();

	PlayBack GetCurrentAnimation() const { return m_currentAnim; }
	PlayBack GetNextAnimation() const { return m_nextAnim; }
	float GetBlendWeight() const { return m_blendWeight; }

	bool IsEnd() const;

	bool TryGetPlaybackTime(const Animation* pAnimation, float& time) const;

	void SetAimAngle(float angle) { m_aimAngle = angle; }

	void ApplyAimRotation();

private:
	Model* m_pModel = nullptr;

	PlayBack m_currentAnim;	// 現在再生中のアニメーション
	PlayBack m_nextAnim;		// 次に再生するアニメーション

	int m_blendFrameCount = 0;	// アニメーション切り替え時のブレンドフレーム数

	float m_blendWeight = 0.0f;	// アニメーション切り替え時のブレンドウェイト(0.0 : 現在のアニメーション100% / 1.0 : 次のアニメーション100%)

	float m_aimAngle = 0.0f;	// 上半身の向き(ラジアン)

	std::vector<int> m_frameIndexes;	// アニメーションを適用するボーンのインデックス
	std::vector<int> m_exclusionFrameIndexes;	// アニメーションを適用しないボーンのインデックス

	std::vector<int> m_aimRotationFrameIndexes;	// 上半身の回転を適用するボーンのインデックス
	std::vector<MATRIX> m_aimRotationDefaultMatrices;	// 上半身の回転を適用するボーンの初期姿勢行列
};