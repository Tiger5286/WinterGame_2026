#pragma once
#include <memory>

class Player;
class GameObjectManager;

namespace SpawnContext
{
	struct Info
	{
		std::shared_ptr<Player> pPlayer = nullptr;
		std::shared_ptr<GameObjectManager> pGameObjectManager = nullptr;
	};
}