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