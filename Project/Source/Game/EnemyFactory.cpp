#include "EnemyFactory.h"
#include <unordered_map>
#include <functional>

#include "Game/GameObjects/Character/Enemies/Walker/Walker.h"
#include "Game/GameObjects/Character/Enemies/Mutant/Mutant.h"

namespace
{
	const std::unordered_map<std::wstring, std::function<std::shared_ptr<EnemyBase>(const std::shared_ptr<Player>&)>> kEnemyNameMap =
	{
		{L"Walker",[](const std::shared_ptr<Player>& player) { return std::make_shared<Walker>(player); }},
		{L"Mutant",[](const std::shared_ptr<Player>& player) { return std::make_shared<Mutant>(player); }},
	};
}

std::shared_ptr<EnemyBase> EnemyFactory::Create(const std::wstring& name, const std::shared_ptr<Player>& player)
{
	auto it = kEnemyNameMap.find(name);
	if (it != kEnemyNameMap.end())
	{
		return it->second(player);
	}
	assert(false && "EnemyFactory::Create() : 指定の敵が生成できませんでした");
	return nullptr;
}