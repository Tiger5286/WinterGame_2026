#include "Animation.h"
#include "DxLib.h"
#include "Resource/Model.h"
#include <cassert>

Animation::Animation(int animIndex, float animSpeed, bool isLoop) :
	m_animIndex(animIndex),
	m_animSpeed(animSpeed),
	m_isLoop(isLoop)
{
}

float Animation::GetAnimTotalTime(Model* pModel) const
{
	if (pModel == nullptr)
	{
		assert(false && "Animation::GetAnimTotalTime() : モデルのポインタがnullptrです");
		return 0.0f;
	}

	return MV1GetAnimTotalTime(pModel->GetHandle(),m_animIndex);
}
