#include "AnimationLayer.h"
#include "Animation.h"
#include <cassert>

namespace
{
	// アニメーション切り替え時のブレンドフレーム数
	constexpr int kAnimationBlendFrame = 10;
}

void AnimationLayer::Play(Animation* pAnimation)
{
	if (pAnimation == nullptr)
	{
		assert(false && "AnimationLayer::Play() : アニメーションのポインタがnullptrです");
		return;
	}

	// 現在再生中のアニメーションと同じアニメーションを再生しようとした場合は、何もしない
	if (m_pCurrentAnimation == pAnimation &&
		m_pNextAnimation == nullptr)
	{
		return;
	}

	// 次のアニメーションがすでに設定されている場合は、現在のアニメーションを次のアニメーションに置き換える
	if (m_pNextAnimation != nullptr)
	{
		m_pCurrentAnimation->Detach();
		m_pCurrentAnimation = m_pNextAnimation;
		m_currentAnimTime = m_nextAnimTime;
		m_blendFrameCount = 0;
		m_pNextAnimation = nullptr;
		m_nextAnimTime = 0.0f;
		m_pCurrentAnimation->Attach();
	}

	// 現在再生中のアニメーションがない場合は、現在のアニメーションとして設定
	if (m_pCurrentAnimation == nullptr)
	{
		m_pCurrentAnimation = pAnimation;
		m_currentAnimTime = 0.0f;
		m_blendFrameCount = 0;
		m_pCurrentAnimation->Attach();
	}
	else	// 現在再生中のアニメーションがある場合は、次のアニメーションとして設定
	{
		m_pNextAnimation = pAnimation;
		m_nextAnimTime = 0.0f;
		m_blendFrameCount = 0;
		m_pNextAnimation->Attach();
	}
}

void AnimationLayer::Apply()
{
	// 現在のアニメーション
	if (m_pCurrentAnimation != nullptr)
	{
		m_pCurrentAnimation->SetTime(m_currentAnimTime);
		m_pCurrentAnimation->SetBlendRate(1.0f - m_blendWeight);
	}

	// 次のアニメーション
	if (m_pNextAnimation != nullptr)
	{
		m_pNextAnimation->SetTime(m_nextAnimTime);
		m_pNextAnimation->SetBlendRate(m_blendWeight);
	}
}

void AnimationLayer::Update()
{
	// アニメーションの再生時間を更新
	if (m_pCurrentAnimation != nullptr)
	{
		m_currentAnimTime += m_pCurrentAnimation->GetAnimSpeed();
		while(m_pCurrentAnimation->IsLoop() && m_pCurrentAnimation->GetAnimTotalTime() < m_currentAnimTime)
		{
			m_currentAnimTime -= m_pCurrentAnimation->GetAnimTotalTime();
		}
	}
	if (m_pNextAnimation != nullptr)
	{
		m_nextAnimTime += m_pNextAnimation->GetAnimSpeed();
		while(m_pNextAnimation->IsLoop() && m_pNextAnimation->GetAnimTotalTime() < m_nextAnimTime)
		{
			m_nextAnimTime -= m_pNextAnimation->GetAnimTotalTime();
		}
	}

	// ブレンドしていない
	if (m_pNextAnimation == nullptr)
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
		// ブレンドが完了したら、次のアニメーションを現在のアニメーションに置き換える
		if (m_pNextAnimation != nullptr)
		{
			m_pCurrentAnimation->Detach();
			m_pCurrentAnimation = m_pNextAnimation;
			m_currentAnimTime = m_nextAnimTime;
			m_pNextAnimation = nullptr;
			m_nextAnimTime = 0.0f;
			m_blendWeight = 0.0f;
		}
	}
}