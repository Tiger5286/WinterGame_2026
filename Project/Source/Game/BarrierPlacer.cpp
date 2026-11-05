#include "BarrierPlacer.h"
#include "Utility/MyLib.h"
#include "Game/GameObjectManager.h"
#include <unordered_map>
#include "Game/GameObjects/Stage/Barrier.h"

namespace
{
	const std::unordered_map<std::wstring, Barrier::ActionType> kActionTypeMap = {
		{ L"Enable", Barrier::ActionType::Enable },
		{ L"Disable", Barrier::ActionType::Disable },
		{ L"Destroy", Barrier::ActionType::Destroy }
	};
}

void BarrierPlacer::Place(const std::shared_ptr<GameObjectManager>& pGameObjectManager, const std::wstring& filePath)
{
	// GameObjectManagerのnullチェック
	if (!pGameObjectManager)
	{
		assert(false && "BarrierPlacer::Place() : pGameObjectManagerがnullptrです");
		return;
	}
	// CSVファイルを読み込む
	auto csv = MyLib::LoadCSV(filePath);
	// CSVの各行を処理
	for (const auto& row : csv)
	{
		// CSVの書き方が正しいかどうかチェック
		// 行の要素数が1以下、または奇数の場合は不正
		if (row.size() <= 1 || row.size() % 2 == 1)
		{
			assert(false && "BarrierPlacer::Place() : CSVの書き方が不正です");
			continue;
		}

		// 1,2列目を読む
		const auto& name = row[0];
		const auto& defaultActionStr = row[1];
		// defaultActionStrがkActionTypeMapに存在するか確認
		auto it = kActionTypeMap.find(defaultActionStr);
		if (it == kActionTypeMap.end())
		{
			assert(false && "BarrierPlacer::Place() : defaultActionTypeが不正です");
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
				assert(false && "BarrierPlacer::Place() : actionTypeが不正です");
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
		pGameObjectManager->Add(barrier);
	}
}
