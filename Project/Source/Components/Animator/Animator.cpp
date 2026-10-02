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

	constexpr int kBlendFrame = 10;
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
	const float blendPerFrame = 1.0f / kBlendFrame;
	if (m_targetSplitWeight == 0.0f)
	{
		m_splitWeight -= blendPerFrame;
		if (m_splitWeight < 0.0f)
		{
			m_splitWeight = 0.0f;
		}
	}
	else if (m_targetSplitWeight == 1.0f)
	{
		m_splitWeight += blendPerFrame;
		if (m_splitWeight > 1.0f)
		{
			m_splitWeight = 1.0f;
		}
	}

	for (size_t i = 0; i < m_animationLayers.size(); ++i)
	{
		auto& layer = m_animationLayers[i];
		// 全身と上下別は逆の重みで混ぜ、各ボーンの合計を1に保つ。
		const float weight = i == static_cast<size_t>(Layer::FullBody)
			? 1.0f - m_splitWeight : m_splitWeight;
		// フェードが完了して影響がなくなった再生枠だけ解放する。
		if (weight == 0.0f)
		{
			layer.Stop();
			continue;
		}
		layer.Update();
		layer.Apply(weight);
	}

	m_animationLayers[static_cast<size_t>(Layer::UpperBody)].ApplyAimRotation();
}

void Animator::Play(const std::wstring& animName, Layer layer)
{
	m_animationLayers[static_cast<size_t>(layer)].Play(&m_animations.at(animName));
}

void Animator::PlaySynced(const std::wstring& animName, Layer source, Layer destination)
{
	auto* pAnimation = &m_animations.at(animName);
	auto& destinationLayer = m_animationLayers[static_cast<size_t>(destination)];
	const auto& sourceLayer = m_animationLayers[static_cast<size_t>(source)];

	float startTime = 0.0f;
	sourceLayer.TryGetPlaybackTime(pAnimation, startTime);

	destinationLayer.Play(pAnimation, startTime);
}

void Animator::Stop(Layer layer)
{
	m_animationLayers[static_cast<size_t>(layer)].Stop();
}

void Animator::SetSplitBody(bool enabled)
{
	if (enabled)
	{
		m_targetSplitWeight = 1.0f;
	}
	else
	{
		m_targetSplitWeight = 0.0f;
	}
}

bool Animator::IsEnd(Layer layer) const
{
	return m_animationLayers[static_cast<size_t>(layer)].IsEnd();
}

void Animator::SetAimAngle(float angle)
{
	m_animationLayers[static_cast<size_t>(Layer::UpperBody)].SetAimAngle(angle);
}