#pragma once
#include <string>
#include <memory>

class GameObjectManager;
class Player;

namespace EnemyPlacer
{
	// 敵を配置するために必要なゲーム内オブジェクト
	struct Info
	{
		std::shared_ptr<Player> pPlayer;
		std::shared_ptr<GameObjectManager> pGameObjectManager;
	};

	/// <summary>
	/// 配置処理。csvファイルを読み込み敵を配置する。
	/// </summary>
	/// <param name="filePath">敵配置情報のファイルパス</param>
	void Place(const Info& info, const std::wstring& filePath);
}