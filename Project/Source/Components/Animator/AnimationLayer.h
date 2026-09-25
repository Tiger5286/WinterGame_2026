#pragma once

class Animation;
class Model;

class AnimationLayer
{
public:
	void Play(Animation* pAnimation);
	void Apply();
	void Update();

	Animation* GetCurrentAnimation() const { return m_pCurrentAnimation; }
	Animation* GetNextAnimation() const { return m_pNextAnimation; }
	float GetCurrentAnimTime() const { return m_currentAnimTime; }
	float GetNextAnimTime() const { return m_nextAnimTime; }
	float GetBlendWeight() const { return m_blendWeight; }

private:
	Animation* m_pCurrentAnimation = nullptr;	// 現在再生中のアニメーション
	Animation* m_pNextAnimation = nullptr;		// 次に再生するアニメーション

	float m_currentAnimTime = 0.0f;	// 現在再生中のアニメーションの再生時間
	float m_nextAnimTime = 0.0f;	// 次に再生するアニメーションの再生時間

	int m_blendFrameCount = 0;	// アニメーション切り替え時のブレンドフレーム数

	float m_blendWeight = 0.0f;	// アニメーション切り替え時のブレンドウェイト(0.0 : 現在のアニメーション100% / 1.0 : 次のアニメーション100%)
};