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

	const std::unordered_map<Type, std::wstring> kInfos = {
		{ Type::Trigger, L"Trigger"},
		{ Type::Critical, L"Critical"}
	};
}