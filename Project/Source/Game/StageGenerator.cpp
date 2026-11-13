#include "StageGenerator.h"
#include "Utility/MyLib.h"
#include "Game/GameObjectManager.h"
#include <unordered_map>
#include "Game/GameObjects/Stage/Barrier.h"
#include "Game/GameObjects/HackPoint.h"
#include "Game/GameObjects/BoxTrigger.h"
#include "Game/GameObjects/BattleArea.h"
#include "Game/EnemyPlacer.h"
#include "Components/Transform.h"

namespace
{
	const std::unordered_map<std::wstring, Barrier::ActionType> kActionTypeMap = {
		{ L"Enable", Barrier::ActionType::Enable },
		{ L"Disable", Barrier::ActionType::Disable },
		{ L"Destroy", Barrier::ActionType::Destroy }
	};

	Vector3 ReadVector3(const std::vector<std::wstring>& row, int index)
	{
		if (index + 2 >= row.size())
		{
			assert(false && "ReadVector3() : 要素外アクセスしようとしています");
			return Vector3::Zero();
		}
		return { std::stof(row[index]),std::stof(row[index + 1]),std::stof(row[index + 2]) };
	}

	// name, defaultType, event, type, event, type, ...
	void PlaceBarriers(const SpawnContext::Info& info, const std::wstring& filePath)
	{
		// CSVファイルを読み込む
		auto csv = MyLib::LoadCSV(filePath);
		// CSVの各行を処理
		for (const auto& row : csv)
		{
			// CSVの書き方が正しいかどうかチェック
			// 行の要素数が1以下、または奇数の場合は不正
			if (row.size() <= 1 || row.size() % 2 == 1)
			{
				assert(false && "PlaceBarriers() : CSVの書き方が不正です");
				continue;
			}

			// 1,2列目を読む
			const auto& name = row[0];
			const auto& defaultActionStr = row[1];
			// defaultActionStrがkActionTypeMapに存在するか確認
			auto it = kActionTypeMap.find(defaultActionStr);
			if (it == kActionTypeMap.end())
			{
				assert(false && "PlaceBarriers() : defaultActionTypeが不正です");
				continue;
			}
			// defaultActionStrをBarrier::ActionTypeに変換
			const auto defaultActionType = it->second;
			// Barrierを生成
			auto barrier = std::make_shared<Barrier>(name, defaultActionType);
			// 3列目以降を読む
			for (int i = 2; i < row.size(); i += 2)
			{
				// 2列ずつ読む。イベント名とアクションタイプのペアを読む
				const auto& eventName = row[i];
				const auto& actionTypeStr = row[i + 1];
				// 同じくactionTypeStrがkActionTypeMapに存在するか確認
				it = kActionTypeMap.find(actionTypeStr);
				if (it == kActionTypeMap.end())
				{
					assert(false && "PlaceBarriers() : actionTypeが不正です");
					continue;
				}
				// actionTypeStrをBarrier::ActionTypeに変換
				const auto actionType = it->second;

				// Barrierにイベントを追加
				Barrier::ActionInfo actionInfo;
				actionInfo.eventName = eventName;
				actionInfo.actionType = actionType;
				barrier->AddEvent(actionInfo);
			}
			// GameObjectManagerにBarrierを追加
			info.pGameObjectManager->Add(barrier);
		}
	}

	// eventName, x, y, z
	void PlaceHackPoints(const SpawnContext::Info& info, const std::wstring& filePath)
	{
		// CSVファイルを読み込む
		auto csv = MyLib::LoadCSV(filePath);

		for (const auto& row : csv)
		{
			const std::wstring& eventName = row[0];
			Vector3 pos = ReadVector3(row, 1);

			auto hackPoint = std::make_shared<HackPoint>();
			hackPoint->SetEvent(eventName);
			hackPoint->GetComponent<Transform>()->pos = pos;
			info.pGameObjectManager->Add(hackPoint);
		}
	}

	// eventName, x, y, z, x, y, z
	void PlaceBoxTriggers(const SpawnContext::Info& info, const std::wstring& filePath)
	{
		auto csv = MyLib::LoadCSV(filePath);

		for (const auto& row : csv)
		{
			const std::wstring& eventName = row[0];
			Vector3 pos1 = ReadVector3(row, 1);
			Vector3 pos2 = ReadVector3(row, 4);

			auto boxTrigger = std::make_shared<BoxTrigger>(pos1, pos2, info.pPlayer, eventName);
			info.pGameObjectManager->Add(boxTrigger);
		}
	}

	// startEventName, endEventName, enemiesDataFilePath
	void PlaceBattleAreas(const SpawnContext::Info& info, const std::wstring& filePath, const std::wstring& folderPath)
	{
		auto csv = MyLib::LoadCSV(filePath);

		for (const auto& row : csv)
		{
			const std::wstring& startEventName = row[0];
			const std::wstring& endEventName = row[1];
			const std::wstring& enemyDataFilePath = row[2];

			// 敵配置情報のcsvのフルパスを作る
			const std::wstring fullPath = folderPath + L"/datas/battleAreas/" + enemyDataFilePath;

			BattleArea::Info battleAreaInfo = {
				.startEventName = startEventName,
				.endEventName = endEventName,
				.csvFilePath = fullPath,
				.pGameObjectManager = info.pGameObjectManager,
				.pPlayer = info.pPlayer
			};
			auto battleArea = std::make_shared<BattleArea>(battleAreaInfo);
			info.pGameObjectManager->Add(battleArea);
		}
	}

	constexpr const wchar_t* kBarrierDataFilePath = L"/datas/barrier.csv";
	constexpr const wchar_t* kHackPointDataFilePath = L"/datas/hackPoint.csv";
	constexpr const wchar_t* kBoxTriggerDataFilePath = L"/datas/boxTrigger.csv";
	constexpr const wchar_t* kBattleAreaDataFilePath = L"/datas/battleArea.csv";
	constexpr const wchar_t* kEnemyDataFilePath = L"/datas/enemy.csv";
}

void StageGenerator::Generate(const SpawnContext::Info& info, const std::wstring& folderPath)
{
	if (!info.pGameObjectManager)
	{
		assert(false && "StageGenerator::Generate() : GameObjectManagerがnullptrです");
		return;
	}
	if (!info.pPlayer)
	{
		assert(false && "StageGenerator::Generate() : Playerがnullptrです");
		return;
	}

	const std::wstring barrierFilePath = folderPath + kBarrierDataFilePath;
	const std::wstring hackPointFilePath = folderPath + kHackPointDataFilePath;
	const std::wstring boxTriggerFilePath = folderPath + kBoxTriggerDataFilePath;
	const std::wstring battleAreaFilePath = folderPath + kBattleAreaDataFilePath;
	const std::wstring enemyFilePath = folderPath + kEnemyDataFilePath;

	PlaceBarriers(info, barrierFilePath);
	PlaceHackPoints(info, hackPointFilePath);
	PlaceBoxTriggers(info, boxTriggerFilePath);
	PlaceBattleAreas(info, battleAreaFilePath, folderPath);
	EnemyPlacer::Place(info, enemyFilePath);
}