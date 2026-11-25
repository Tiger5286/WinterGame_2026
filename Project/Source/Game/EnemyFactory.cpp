#include "EnemyFactory.h"
#include <unordered_map>
#include <functional>
#include "Game/GameObjects/Character/Enemies/Walker/Walker.h"
#include "Game/GameObjects/Character/Enemies/Mutant/Mutant.h"
#include "Game/GameObjects/Character/Enemies/Drone/Drone.h"

namespace
{
	const std::unordered_map<std::wstring, std::function<std::shared_ptr<EnemyBase>(const SpawnContext::Info&)>> kEnemyNameMap =
	{
		{L"Walker",[](const SpawnContext::Info& info) { return std::make_shared<Walker>(info); }},
		{L"Mutant",[](const SpawnContext::Info& info) { return std::make_shared<Mutant>(info); }},
		{L"Drone",[](const SpawnContext::Info& info) { return std::make_shared<Drone>(info); }},
	};
}

std::shared_ptr<EnemyBase> EnemyFactory::Create(const std::wstring& name, const SpawnContext::Info& info)
{
	auto it = kEnemyNameMap.find(name);
	if (it != kEnemyNameMap.end())
	{
		return it->second(info);
	}
	assert(false && "EnemyFactory::Create() : 指定の敵が生成できませんでした");
	return nullptr;
}