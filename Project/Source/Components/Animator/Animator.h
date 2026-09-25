#pragma once
#include "../Component.h"
#include <unordered_map>
#include <string>
#include "Animation.h"
#include "AnimationLayer.h"
#include <array>

class Model;

class Animator : public Component
{
public:
	enum class Layer
	{
		FullBody,
		//UpperBody,
		//LowerBody,

		Num
	};

public:
	Animator() = default;
	~Animator() = default;

	void Init(Model* pModel);

	void AddAnimation(const std::wstring& animName, float animSpeed = 0.5f, bool isLoop = true);

	void Update();

	void Play(const std::wstring& animName, Layer layer = Layer::FullBody);

private:
	Model* m_pModel = nullptr;
	std::unordered_map<std::wstring, Animation> m_animations;
	std::array<AnimationLayer, static_cast<size_t>(Layer::Num)> m_animationLayers;
};

