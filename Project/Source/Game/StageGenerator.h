#pragma once
#include <string>
#include <memory>

class GameObjectManager;
class Player;

namespace StageGenerator
{
	// オブジェクトを生成するために必要な情報をまとめた構造体
	struct Info
	{
		std::shared_ptr<Player> pPlayer;
		std::shared_ptr<GameObjectManager> pGameObjectManager;
	};

	/// <summary>
	/// csvファイルを読み込み、障害物を配置する
	/// </summary>
	/// <param name="pGameObjectManager">GameObjectManagerのポインタ</param>
	/// <param name="filePath">csvファイルのパス</param>
	void PlaceBarriers(const std::shared_ptr<GameObjectManager>& pGameObjectManager, const std::wstring& filePath);

	/// <summary>
	/// csvファイルを読み込み、ハックポイントを配置する
	/// </summary>
	/// <param name="info">必要な情報をまとめた構造体Info</param>
	/// <param name="filePath">csvファイルのパス</param>
	void PlaceHackPoints(const Info& info, const std::wstring& filePath);

	/// <summary>
	/// csvファイルを読み込み、BoxTriggerを配置する
	/// </summary>
	/// <param name="info">必要な情報をまとめた構造体Info</param>
	/// <param name="filePath">csvファイルのパス</param>
	void PlaceBoxTriggers(const Info& info, const std::wstring& filePath);

	/// <summary>
	/// csvファイルを読み込み、バトルエリアを生成する
	/// </summary>
	/// <param name="info">必要な情報をまとめた構造体Info</param>
	/// <param name="filePath">csvファイルのパス</param>
	void PlaceBattleAreas(const Info& info, const std::wstring& filePath);
}