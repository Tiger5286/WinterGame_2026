#include "Animator.h"
#include "DxLib.h"
#include "assert.h"
#include "Resource/Model.h"
#include <vector>
#include <string>

namespace
{
	const std::vector<std::wstring> kFullBodyBones = { L"mixamorig:Hips" };
	const std::vector<std::wstring> kFullBodyExclusionBones = {};

	const std::vector<std::wstring> kUpperBodyBones = { L"mixamorig:Hips" };
	const std::vector<std::wstring> kUpperBodyExclusionBones = { L"mixamorig:LeftUpLeg", L"mixamorig:RightUpLeg" };

	const std::vector<std::wstring> kLowerBodyBones = { L"mixamorig:LeftUpLeg", L"mixamorig:RightUpLeg" };
	const std::vector<std::wstring> kLowerBodyExclusionBones = {};
}

void Animator::Init(Model* pModel)
{
	m_pModel = pModel;
	m_animationLayers[static_cast<int>(Layer::FullBody)].Init(m_pModel, kFullBodyBones,kFullBodyExclusionBones);
	m_animationLayers[static_cast<int>(Layer::UpperBody)].Init(m_pModel, kUpperBodyBones,kUpperBodyExclusionBones);
	m_animationLayers[static_cast<int>(Layer::LowerBody)].Init(m_pModel, kLowerBodyBones,kLowerBodyExclusionBones);
}

void Animator::AddAnimation(const std::wstring& animName, float animSpeed, bool isLoop)
{
	if (m_animations.find(animName) != m_animations.end())
	{
		assert(false && "Animator::AddAnimation() : 同じ名前のアニメーションを追加しようとしています");
		return;
	}

	int animIndex = MV1GetAnimIndex(m_pModel->GetHandle(), animName.c_str());
	if (animIndex == -1)
	{
		assert(false && "Animator::AddAnimation() : AnimIndexを取得できませんでした");
		return;
	}

	m_animations.try_emplace(animName, *m_pModel, animIndex, animSpeed, isLoop);
}

void Animator::Update()
{
	for (auto& layer : m_animationLayers)
	{
		layer.Update();
		layer.Apply();
	}
}

void Animator::Play(const std::wstring& animName, Layer layer)
{
	m_animationLayers[static_cast<size_t>(layer)].Play(&m_animations.at(animName));
}

void Animator::Stop(Layer layer)
{
	m_animationLayers[static_cast<size_t>(layer)].Stop();
}

bool Animator::IsEnd(Layer layer) const
{
	return m_animationLayers[static_cast<size_t>(layer)].IsEnd();
}