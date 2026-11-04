#include "EnemyPlacer.h"
#include "Game/GameObjectManager.h"
#include "Game/GameObjects/Character/Player/Player.h"
#include "Game/GameObjects/Character/Enemies/Walker/Walker.h"
#include "Game/GameObjects/Character/Enemies/Mutant/Mutant.h"
#include "Utility/MyLib.h"
#include "Components/Transform.h"

namespace
{
	const std::unordered_map<std::wstring, std::function<std::shared_ptr<EnemyBase>(const std::shared_ptr<Player>&)>> kEnemyNameMap =
	{
		{L"Walker",[](const std::shared_ptr<Player>& player) { return std::make_shared<Walker>(player); }}
	};
}

void EnemyPlacer::Place(const Info& info, const std::wstring& filePath)
{
	auto datas = MyLib::LoadCSV(filePath);

	for (const auto& data : datas)
	{
		auto it = kEnemyNameMap.find(data[0]);
		if (it == kEnemyNameMap.end())
		{
			assert(false && "EnemyPlacer::Place() : 敵の名前が正しくありません");
			continue;
		}
		auto enemy = it->second(info.pPlayer);
		Vector3 pos;
		pos.x = std::stof(data[1]);
		pos.y = std::stof(data[2]);
		pos.z = std::stof(data[3]);
		enemy->GetComponent<Transform>()->pos = pos;
		info.pGameObjectManager->Add(enemy);
	}
}