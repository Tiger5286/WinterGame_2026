#include "Animator.h"
#include "DxLib.h"
#include "assert.h"
#include "Resource/Model.h"

Animator::Animator(Model* pModel) :
	m_pModel(pModel)
{
	if (m_pModel == nullptr)
	{
		assert(false && "Animator::Animator() : モデルのポインタがnullptrです");
	}
}

void Animator::AddAnimation(const std::wstring& animName, float animSpeed, bool isLoop)
{
	if (m_pModel == nullptr)
	{
		assert(false && "Animator::AddAnimation() : モデルのポインタがnullptrです");
		return;
	}

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

	m_animations.try_emplace(animName, animIndex, animSpeed, isLoop);
}

void Animator::Update()
{
	if (m_pModel == nullptr)
	{
		assert(false && "Animator::Update() : モデルのポインタがnullptrです");
		return;
	}

	for (auto& layer : m_animationLayers)
	{
		layer.Update(m_pModel);
	}
}

void Animator::Play(const std::wstring& animName, Layer layer)
{
	m_animationLayers[static_cast<size_t>(layer)].Play(&m_animations.at(animName),m_pModel);
}
