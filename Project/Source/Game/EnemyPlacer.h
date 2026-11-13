#pragma once
#include <string>
#include <memory>
#include <vector>
#include "SpawnContext.h"

class EnemyBase;

namespace EnemyPlacer
{
	/// <summary>
	/// 敵の配置処理。csvファイルを読み込み、敵を生成して配置する
	/// </summary>
	/// <param name="info">敵を配置するために必要な情報を持った構造体Info</param>
	/// <param name="filePath">csvファイルのパス</param>
	/// <returns>配置した敵の配列</returns>
	std::vector<std::shared_ptr<EnemyBase>> Place(const SpawnContext::Info& info, const std::wstring& filePath);
}