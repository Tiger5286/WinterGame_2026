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

void AnimationLayer::Play(Animation* pAnimation,Model* pModel)
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
		MV1DetachAnim(pModel->GetHandle(), m_pCurrentAnimation->GetAnimIndex());
		m_pCurrentAnimation = m_pNextAnimation;
		m_currentAnimTime = m_nextAnimTime;
		m_blendFrameCount = 0;
		m_pNextAnimation = nullptr;
		m_nextAnimTime = 0.0f;
		MV1AttachAnim(pModel->GetHandle(), m_pCurrentAnimation->GetAnimIndex());
	}

	// 現在再生中のアニメーションがない場合は、現在のアニメーションとして設定
	if (m_pCurrentAnimation == nullptr)
	{
		m_pCurrentAnimation = pAnimation;
		m_currentAnimTime = 0.0f;
		m_blendFrameCount = 0;
		MV1AttachAnim(pModel->GetHandle(), m_pCurrentAnimation->GetAnimIndex());
	}
	else	// 現在再生中のアニメーションがある場合は、次のアニメーションとして設定
	{
		m_pNextAnimation = pAnimation;
		m_nextAnimTime = 0.0f;
		m_blendFrameCount = 0;
		MV1AttachAnim(pModel->GetHandle(), m_pNextAnimation->GetAnimIndex());
	}
}

void AnimationLayer::Apply(Model* pModel)
{
	if (pModel == nullptr)
	{
		assert(false && "AnimationLayer::Apply() : モデルのポインタがnullptrです");
		return;
	}

	const int modelHandle = pModel->GetHandle();

	// Current
	if (m_pCurrentAnimation != nullptr)
	{
		int attachIndex =
			m_pCurrentAnimation->GetAnimIndex();

		MV1SetAttachAnimTime(
			modelHandle,
			attachIndex,
			m_currentAnimTime);

		MV1SetAttachAnimBlendRate(
			modelHandle,
			attachIndex,
			1.0f - m_blendWeight);
	}

	// Next
	if (m_pNextAnimation != nullptr)
	{
		int attachIndex =
			m_pNextAnimation->GetAnimIndex();

		MV1SetAttachAnimTime(
			modelHandle,
			attachIndex,
			m_nextAnimTime);

		MV1SetAttachAnimBlendRate(
			modelHandle,
			attachIndex,
			m_blendWeight);
	}
}

void AnimationLayer::Update(Model* pModel)
{
	if (pModel == nullptr)
	{
		assert(false && "AnimationLayer::Update() : モデルのポインタがnullptrです");
		return;
	}

	// アニメーションの再生時間を更新
	if (m_pCurrentAnimation != nullptr)
	{
		m_currentAnimTime += m_pCurrentAnimation->GetAnimSpeed();
		while(m_pCurrentAnimation->IsLoop() && m_pCurrentAnimation->GetAnimTotalTime(pModel) < m_currentAnimTime)
		{
			m_currentAnimTime -= m_pCurrentAnimation->GetAnimTotalTime(pModel);
		}
	}
	if (m_pNextAnimation != nullptr)
	{
		m_nextAnimTime += m_pNextAnimation->GetAnimSpeed();
		while(m_pNextAnimation->IsLoop() && m_pNextAnimation->GetAnimTotalTime(pModel) < m_nextAnimTime)
		{
			m_nextAnimTime -= m_pNextAnimation->GetAnimTotalTime(pModel);
		}
	}

	// ブレンドしていない
	if (m_pNextAnimation == nullptr)
	{
		m_blendWeight = 1.0f;
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
			MV1DetachAnim(pModel->GetHandle(), m_pCurrentAnimation->GetAnimIndex());
			m_pCurrentAnimation = m_pNextAnimation;
			m_currentAnimTime = m_nextAnimTime;
			m_pNextAnimation = nullptr;
			m_nextAnimTime = 0.0f;
		}
	}
}