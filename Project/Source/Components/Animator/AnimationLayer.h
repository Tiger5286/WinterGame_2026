#pragma once
#include <vector>
#include <string>

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

	void Play(Animation* pAnimation);
	void Stop();
	void Apply();
	void Update();

	PlayBack GetCurrentAnimation() const { return m_currentAnim; }
	PlayBack GetNextAnimation() const { return m_nextAnim; }
	float GetBlendWeight() const { return m_blendWeight; }

	bool IsEnd() const;

private:
	Model* m_pModel = nullptr;

	PlayBack m_currentAnim;	// 現在再生中のアニメーション
	PlayBack m_nextAnim;		// 次に再生するアニメーション

	int m_blendFrameCount = 0;	// アニメーション切り替え時のブレンドフレーム数

	float m_blendWeight = 0.0f;	// アニメーション切り替え時のブレンドウェイト(0.0 : 現在のアニメーション100% / 1.0 : 次のアニメーション100%)

	std::vector<int> m_frameIndexes;
	std::vector<int> m_exclusionFrameIndexes;
};