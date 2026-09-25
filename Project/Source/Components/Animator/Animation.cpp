#include "Animation.h"
#include "DxLib.h"
#include "Resource/Model.h"
#include <cassert>

Animation::Animation(Model& model,int animIndex, float animSpeed, bool isLoop) :
	m_model(model),
	m_animIndex(animIndex),
	m_animSpeed(animSpeed),
	m_isLoop(isLoop),
	m_totalTime(MV1GetAnimTotalTime(m_model.GetHandle(), m_animIndex))
{
}

void Animation::Attach()
{
	// すでにアタッチされている場合はアタッチしない
	if (m_attachIndex != -1)
	{
		return;
	}

	m_attachIndex = MV1AttachAnim(m_model.GetHandle(), m_animIndex);
	if (m_attachIndex == -1)
	{
		assert(false && "Animation::Attach() : アニメーションが正しくアタッチされませんでした");
	}
}

void Animation::Detach()
{
	// アタッチされていないならデタッチしない
	if (m_attachIndex == -1)
	{
		return;
	}

	MV1DetachAnim(m_model.GetHandle(), m_attachIndex);
	m_attachIndex = -1;
}

void Animation::SetTime(float time)
{
	// アタッチされていないなら処理しない
	if (m_attachIndex == -1)
	{
		return;
	}

	MV1SetAttachAnimTime(m_model.GetHandle(), m_attachIndex, time);
}

void Animation::SetBlendRate(float rate)
{
	// アタッチされていないなら処理しない
	if (m_attachIndex == -1)
	{
		return;
	}

	MV1SetAttachAnimBlendRate(m_model.GetHandle(), m_attachIndex, rate);
}
