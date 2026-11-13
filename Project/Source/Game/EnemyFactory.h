#pragma once
#include <memory>
#include <string>
#include "SpawnContext.h"

class EnemyBase;
class Player;
class GameObjectManager;

namespace EnemyFactory
{	
	/// <summary>
	/// 名前から敵を生成する
	/// </summary>
	/// <param name="name">敵の名前</param>
	/// <param name="info">必要な情報</param>
	/// <returns>生成した敵。名前が不正ならnullptrを返す</returns>
	std::shared_ptr<EnemyBase> Create(const std::wstring& name,const SpawnContext::Info& info);
}
