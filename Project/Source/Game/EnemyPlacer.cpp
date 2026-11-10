#include "EnemyPlacer.h"
#include "Game/GameObjectManager.h"
#include "EnemyFactory.h"
#include "Utility/MyLib.h"
#include "Components/Transform.h"
#include "GameObjects/Character/Enemies/EnemyBase.h"

std::vector<std::shared_ptr<EnemyBase>> EnemyPlacer::Place(const EnemyFactory::Info& info, const std::wstring& filePath)
{
	auto datas = MyLib::LoadCSV(filePath);

	std::vector<std::shared_ptr<EnemyBase>> enemies;

	for (const auto& data : datas)
	{
		auto enemy = EnemyFactory::Create(data[0], info);
		if (!enemy)
		{
			assert(false && "EnemyPlacer::Place() : EnemyFactory::Create()に失敗しました");
			continue;
		}
		Vector3 pos;
		pos.x = std::stof(data[1]);
		pos.y = std::stof(data[2]);
		pos.z = std::stof(data[3]);
		enemy->GetComponent<Transform>()->pos = pos;
		info.pGameObjectManager->Add(enemy);
		enemies.push_back(enemy);
	}

	return enemies;
}