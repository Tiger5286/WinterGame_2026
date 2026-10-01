#include "AnimationLayer.h"
#include "Animation.h"
#include <cassert>
#include "DxLib.h"
#include "Resource/Model.h"

namespace
{
	// アニメーション切り替え時のブレンドフレーム数
	constexpr int kAnimationBlendFrame = 10;
}
void AnimationLayer::Init(Model* pModel, const std::vector<std::wstring>& boneNames, const std::vector<std::wstring>& exclusionBoneNames)
{
	m_pModel = pModel;
	for (const auto& boneName : boneNames)
	{
		m_frameIndexes.push_back(MV1SearchFrame(m_pModel->GetHandle(), boneName.c_str()));
		assert(m_frameIndexes.back() != -1 && "AnimationLayer::Init() : ボーンが正しく取得できませんでした");
	}
	for (const auto& boneName : exclusionBoneNames)
	{
		m_exclusionFrameIndexes.push_back(MV1SearchFrame(m_pModel->GetHandle(), boneName.c_str()));
		assert(m_exclusionFrameIndexes.back() != -1 && "AnimationLayer::Init() : ボーンが正しく取得できませんでした");
	}
}

void AnimationLayer::Play(Animation* pAnimation,float startTime)
{
	if (pAnimation == nullptr)
	{
		assert(false && "AnimationLayer::Play() : アニメーションのポインタがnullptrです");
		return;
	}

	// 再生中のアニメーションと同じアニメーションを再生しようとした場合は、何もしない
	if (m_currentAnim.pAnimation == pAnimation ||
		m_nextAnim.pAnimation == pAnimation)
	{
		return;
	}

	// 次のアニメーションがすでに設定されている場合は、現在のアニメーションを次のアニメーションに置き換える
	if (m_nextAnim.pAnimation != nullptr)
	{
		// 引き継ぐ前に、古いcurrentのアタッチを解放する。
		MV1DetachAnim(m_pModel->GetHandle(), m_currentAnim.attachIndex);
		// 構造体の代入でポインタ・アタッチ番号・時間をまとめて引き継ぐ。
		m_currentAnim = m_nextAnim;
		// next側の情報だけ初期化する。引き継いだアタッチは解放しない。
		m_nextAnim = {};
	}

	// 現在再生中のアニメーションがない場合は、現在のアニメーションとして設定
	if (m_currentAnim.pAnimation == nullptr)
	{
		// 省略したメンバには既定値（attachIndex=-1、time=0）が入る。
		m_currentAnim = PlayBack{ pAnimation,-1,startTime };
		// アタッチして、戻り値をm_currentAnim.attachIndexに保存する。
		m_currentAnim.attachIndex = MV1AttachAnim(m_pModel->GetHandle(), m_currentAnim.pAnimation->GetAnimIndex());
	}
	else	// 現在再生中のアニメーションがある場合は、次のアニメーションとして設定
	{
		m_nextAnim = PlayBack{ pAnimation,-1,startTime };
		// アタッチして、戻り値をm_nextAnim.attachIndexに保存する。
		m_nextAnim.attachIndex = MV1AttachAnim(m_pModel->GetHandle(), m_nextAnim.pAnimation->GetAnimIndex());
	}

	// 新しい再生要求を受け付けたときだけ、切り替えの進行状態をリセットする。
	m_blendFrameCount = 0;
	m_blendWeight = 0.0f;
}

void AnimationLayer::Stop()
{
	// 未再生・停止済みのレイヤーにも安全に呼べるよう、有効な番号だけ解放する。
	if (m_pModel && m_currentAnim.attachIndex != -1)
	{
		MV1DetachAnim(m_pModel->GetHandle(), m_currentAnim.attachIndex);
	}
	m_currentAnim = {};
	if (m_pModel && m_nextAnim.attachIndex != -1)
	{
		MV1DetachAnim(m_pModel->GetHandle(), m_nextAnim.attachIndex);
	}
	m_nextAnim = {};
	m_blendFrameCount = 0;
	m_blendWeight = 0.0f;
}

void AnimationLayer::Apply(float layerWeight)
{
	// 現在のアニメーション
	if (m_currentAnim.pAnimation != nullptr)
	{
		// currentのattachIndexにtimeとブレンド率(1.0f - m_blendWeight)を反映する。
		MV1SetAttachAnimTime(m_pModel->GetHandle(), m_currentAnim.attachIndex, m_currentAnim.time);
		// 一旦全身のブレンド率を0にする
		MV1SetAttachAnimBlendRate(m_pModel->GetHandle(), m_currentAnim.attachIndex, 0.0f);
		// 指定のボーンにだけアニメーションをブレンド
		for (const auto& frame : m_frameIndexes)
		{
			MV1SetAttachAnimBlendRateToFrame(m_pModel->GetHandle(), m_currentAnim.attachIndex, frame, (1.0f - m_blendWeight) * layerWeight);
		}
		// 指定のボーンのアニメーションのブレンド率を0にする
		for (const auto& frame : m_exclusionFrameIndexes)
		{
			MV1SetAttachAnimBlendRateToFrame(m_pModel->GetHandle(), m_currentAnim.attachIndex, frame, 0.0f);
		}
	}

	// 次のアニメーション
	if (m_nextAnim.pAnimation != nullptr)
	{
		// nextのattachIndexにtimeとブレンド率m_blendWeightを反映する。
		MV1SetAttachAnimTime(m_pModel->GetHandle(), m_nextAnim.attachIndex, m_nextAnim.time);
		// 一旦全身のブレンド率0にする
		MV1SetAttachAnimBlendRate(m_pModel->GetHandle(), m_nextAnim.attachIndex, 0.0f);
		for (const auto& frame : m_frameIndexes)
		{
			MV1SetAttachAnimBlendRateToFrame(m_pModel->GetHandle(), m_nextAnim.attachIndex, frame, m_blendWeight * layerWeight);
		}
		// 指定のボーンのアニメーションのブレンド率を0にする
		for (const auto& frame : m_exclusionFrameIndexes)
		{
			MV1SetAttachAnimBlendRateToFrame(m_pModel->GetHandle(), m_nextAnim.attachIndex, frame, 0.0f);
		}
	}
}

void AnimationLayer::Update()
{
	// アニメーションの再生時間を更新
	if (m_currentAnim.pAnimation != nullptr)
	{
		m_currentAnim.time += m_currentAnim.pAnimation->GetAnimSpeed();
		while(m_currentAnim.pAnimation->IsLoop() && m_currentAnim.pAnimation->GetAnimTotalTime() < m_currentAnim.time)
		{
			m_currentAnim.time -= m_currentAnim.pAnimation->GetAnimTotalTime();
		}
	}
	if (m_nextAnim.pAnimation != nullptr)
	{
		m_nextAnim.time += m_nextAnim.pAnimation->GetAnimSpeed();
		while(m_nextAnim.pAnimation->IsLoop() && m_nextAnim.pAnimation->GetAnimTotalTime() < m_nextAnim.time)
		{
			m_nextAnim.time -= m_nextAnim.pAnimation->GetAnimTotalTime();
		}
	}

	// ブレンドしていない
	if (m_nextAnim.pAnimation == nullptr)
	{
		m_blendWeight = 0.0f;
		return;
	}

	// アニメーションのブレンド処理
	if (m_blendFrameCount < kAnimationBlendFrame)
	{
		m_blendFrameCount++;
		m_blendWeight = static_cast<float>(m_blendFrameCount) / kAnimationBlendFrame;
	}
	else
	{
		// nextがない場合は上でreturnしているため、ここでの再確認は不要。
		// 引き継ぐ前に、古いcurrentのアタッチを解放する。
		MV1DetachAnim(m_pModel->GetHandle(), m_currentAnim.attachIndex);
		m_currentAnim = m_nextAnim;
		m_nextAnim = {};
		m_blendFrameCount = 0;
		m_blendWeight = 0.0f;
	}
}

bool AnimationLayer::IsEnd() const
{
	if (m_nextAnim.pAnimation)
	{
		if (!m_nextAnim.pAnimation) return false;

		if (m_nextAnim.pAnimation->IsLoop()) return false;

		if (m_nextAnim.time < m_nextAnim.pAnimation->GetAnimTotalTime()) return false;

		return true;
	}
	else
	{
		if (!m_currentAnim.pAnimation) return false;

		if (m_currentAnim.pAnimation->IsLoop()) return false;

		if (m_currentAnim.time < m_currentAnim.pAnimation->GetAnimTotalTime()) return false;

		return true;
	}
}

bool AnimationLayer::TryGetPlaybackTime(const Animation* pAnimation, float& time) const
{
	// 切り替え中ならnextの再生時間を優先する
	if (m_nextAnim.pAnimation == pAnimation)
	{
		time = m_nextAnim.time;
		return true;
	}

	// 切り替え中でない場合はcurrentの再生時間を返す
	if (m_currentAnim.pAnimation == pAnimation)
	{
		time = m_currentAnim.time;
		return true;
	}

	// どちらのアニメーションでもない場合はfalseを返す
	return false;
}
