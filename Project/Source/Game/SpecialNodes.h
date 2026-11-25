#pragma once
#include <unordered_map>
#include <string>

namespace SpecialNodes
{
	enum class Type
	{
		None,
		Trigger,	// トリガーノード(銃の連射速度が上がる)
		Critical,	// クリティカルノード(ハッキング成功時のダメージが上がる)

		Num
	};

	inline const std::unordered_map<Type, std::wstring> kInfos = {
		{ Type::Trigger, L"Trigger"},
		{ Type::Critical, L"Critical"}
	};

	inline Type GetSpecialNodeTypeRandom()
	{
		constexpr int minVal = 1;
		constexpr int maxVal = static_cast<int>(SpecialNodes::Type::Num) - 1;
		SpecialNodes::Type type = static_cast<SpecialNodes::Type>(minVal + GetRand(maxVal - minVal));

		return type;
	}
}