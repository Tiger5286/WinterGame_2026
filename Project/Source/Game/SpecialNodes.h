#pragma once
#include <unordered_map>
#include <string>
#include "DxLib.h"

namespace SpecialNodes
{
	enum class Type
	{
		None,
		Trigger,	// トリガーノード(銃の連射速度が上がる)
		Critical,	// クリティカルノード(ハッキング成功時のダメージが上がる)

		Num
	};

	struct Info
	{
		std::wstring name;
		std::wstring graphKey;
	};

	inline const std::unordered_map<Type, Info> kInfos = {
		{ Type::Trigger,	{ L"トリガー",		L"TriggerIcon"	}},
		{ Type::Critical,	{ L"クリティカル",	L"CriticalIcon"	}}
	};

	inline Type GetSpecialNodeTypeRandom()
	{
		constexpr int minVal = 1;
		constexpr int maxVal = static_cast<int>(SpecialNodes::Type::Num) - 1;
		SpecialNodes::Type type = static_cast<SpecialNodes::Type>(minVal + GetRand(maxVal - minVal));

		return type;
	}
}