#pragma once
#include "../Component.h"
#include <string>

class Model;

/// <summary>
/// 骨(Mixamoのボーン)を持たないモデル用のアニメーション再生コンポーネント
/// アニメーションを1つだけアタッチして再生する(ブレンドや上下半身の分割はしない)
/// </summary>
class SimpleAnimator : public Component
{
public:
	SimpleAnimator() = default;
	~SimpleAnimator();

	void Init(Model* pModel);

	// 指定した名前のアニメーションを再生する
	void Play(const std::wstring& animName, float animSpeed = 0.5f, bool isLoop = true);

	// 再生時間を進めてモデルに反映する
	void Update(float timeScale);

	// trueで再生時間を止める(アタッチは外さないので、止めた時のポーズのまま固まる)
	void Pause(bool isPause) { m_isPause = isPause; }

	// ループしないアニメーションが最後まで再生されたらtrue
	bool IsEnd() const;

private:
	// アタッチしているアニメーションを外す
	void Detach();

private:
	Model* m_pModel = nullptr;
	int m_attachIndex = -1;		// アタッチ番号(-1ならアタッチしていない)
	float m_time = 0.0f;		// 現在の再生時間
	float m_totalTime = 0.0f;	// アニメーションの総時間
	float m_animSpeed = 0.5f;	// 1フレームに進める時間
	bool m_isLoop = true;
	bool m_isPause = false;
};
