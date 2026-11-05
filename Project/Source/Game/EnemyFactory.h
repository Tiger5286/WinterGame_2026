#pragma once
#include <memory>
#include <string>

class EnemyBase;
class Player;

namespace EnemyFactory
{
	/// <summary>
	/// 名前から敵を生成する
	/// </summary>
	/// <param name="name">敵の名前</param>
	/// <param name="player">プレイヤーのポインタ</param>
	/// <returns>生成した敵。名前が不正ならnullptrを返す</returns>
	std::shared_ptr<EnemyBase> Create(const std::wstring& name,const std::shared_ptr<Player>& player);
}
