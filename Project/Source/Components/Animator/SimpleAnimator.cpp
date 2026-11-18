#include "SimpleAnimator.h"
#include "DxLib.h"
#include <cassert>
#include "Resource/Model.h"

SimpleAnimator::~SimpleAnimator()
{
	// モデルは持ち主が先に破棄することがあるので、ここではデタッチしない
	// (モデルを削除するとアタッチしたアニメーションも一緒に消える)
}

void SimpleAnimator::Init(Model* pModel)
{
	m_pModel = pModel;
}

void SimpleAnimator::Play(const std::wstring& animName, float animSpeed, bool isLoop)
{
	if (!m_pModel)
	{
		assert(false && "SimpleAnimator::Play() : Initでモデルが設定されていません");
		return;
	}

	const int animIndex = MV1GetAnimIndex(m_pModel->GetHandle(), animName.c_str());
	if (animIndex == -1)
	{
		assert(false && "SimpleAnimator::Play() : AnimIndexを取得できませんでした");
		return;
	}

	// すでに再生中のアニメーションがあれば外してから付け直す
	Detach();

	m_attachIndex = MV1AttachAnim(m_pModel->GetHandle(), animIndex);
	m_totalTime = MV1GetAttachAnimTotalTime(m_pModel->GetHandle(), m_attachIndex);
	m_time = 0.0f;
	m_animSpeed = animSpeed;
	m_isLoop = isLoop;
	m_isPause = false;
}

void SimpleAnimator::Update()
{
	if (m_attachIndex == -1) return;

	if (!m_isPause)
	{
		m_time += m_animSpeed;
		if (m_isLoop)
		{
			// ループするなら総時間を超えた分だけ最初に戻す
			while (m_totalTime > 0.0f && m_time > m_totalTime)
			{
				m_time -= m_totalTime;
			}
		}
		else if (m_time > m_totalTime)
		{
			// ループしないなら最後で止める
			m_time = m_totalTime;
		}
	}

	MV1SetAttachAnimTime(m_pModel->GetHandle(), m_attachIndex, m_time);
}

bool SimpleAnimator::IsEnd() const
{
	if (m_attachIndex == -1) return false;
	if (m_isLoop) return false;
	return m_time >= m_totalTime;
}

void SimpleAnimator::Detach()
{
	if (m_pModel && m_attachIndex != -1)
	{
		MV1DetachAnim(m_pModel->GetHandle(), m_attachIndex);
	}
	m_attachIndex = -1;
}
