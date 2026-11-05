#include "EnemyPlacer.h"
#include "Game/GameObjectManager.h"
#include "EnemyFactory.h"
#include "Utility/MyLib.h"
#include "Components/Transform.h"
#include "GameObjects/Character/Enemies/EnemyBase.h"

void EnemyPlacer::Place(const Info& info, const std::wstring& filePath)
{
	auto datas = MyLib::LoadCSV(filePath);

	for (const auto& data : datas)
	{
		auto enemy = EnemyFactory::Create(data[0], info.pPlayer);
		Vector3 pos;
		pos.x = std::stof(data[1]);
		pos.y = std::stof(data[2]);
		pos.z = std::stof(data[3]);
		enemy->GetComponent<Transform>()->pos = pos;
		info.pGameObjectManager->Add(enemy);
	}
}