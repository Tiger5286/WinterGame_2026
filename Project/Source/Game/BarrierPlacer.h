#pragma once
#include <string>
#include <memory>

class GameObjectManager;

namespace BarrierPlacer
{
	/// <summary>
	/// csvファイルを読み込み、障害物を配置する
	/// </summary>
	/// <param name="pGameObjectManager">GameObjectManagerのポインタ</param>
	/// <param name="filePath">csvファイルのパス</param>
	void Place(const std::shared_ptr<GameObjectManager>& pGameObjectManager, const std::wstring& filePath);
}