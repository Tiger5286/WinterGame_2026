#include "SceneMain.h"
#include "DxLib.h"
#include <string>
#include "Game.h"
#include "Game/GameObjectManager.h"
#include "Game/GameObjects/Camera/Camera.h"
#include "Game/GameObjects/Character/Player/Player.h"
#include "Game/GameObjects/Character/Enemies/TestEnemy.h"
#include "Game/GameObjects/Stage/StaticStage.h"
#include "Game/GameObjects/HackPoint.h"
#include "Game/GameObjects/Stage/Barrier.h"
#include "Game/CollisionManager.h"
#include "Resource/ResourceManager.h"
#include "System/ServiceLocator.h"
#include "Components/Component.h"
#include "Components/Transform.h"
#include "Game/HackingManager.h"
#include "System/UIManager.h"
#include "SceneResult.h"
#include "SceneManager.h"
#include "System/PadInput.h"
#include "Game/EnemyPlacer.h"
#include "Game/GameObjects/BoxTrigger.h"
#include "Game/StageGenerator.h"
#include "Game/GameObjects/BattleArea.h"
#include "System/EventManager.h"
#include "ScenePause.h"

namespace
{
	//const std::wstring kResourceFilePath = L"data/stages/stage1/stage1Resources.csv";

	constexpr const wchar_t* kClearEventName = L"DefeatedBoss";

	struct LoadCSVDatas
	{
		const wchar_t* resources;
		const wchar_t* barriers;
		const wchar_t* enemies;
		const wchar_t* boxTriggers;
		const wchar_t* battleAreas;
	};

	const LoadCSVDatas kStageDatas[] = {
		{
			.resources = L"data/stages/stage1/datas/stage1Resources.csv",
			.barriers = L"data/stages/stage1/datas/barrierData.csv",
			.enemies = L"data/stages/stage1/datas/enemyPlaceData.csv",
			.boxTriggers = L"data/stages/stage1/datas/boxTriggerData.csv",
			.battleAreas = L"data/stages/stage1/datas/battleAreas.csv"
		}
	};
}

SceneMain::SceneMain(SceneManager& sceneManager,int stage):
	SceneBase(sceneManager),
	m_nowStage(stage)
{
}

SceneMain::~SceneMain()
{
	// CollisionManagerが消えるのでServiceLocatorの登録を解除
	ServiceLocator::GetInstance().ProvideCollisionManager(nullptr);
	// SceneMainでしか使わないリソースを削除
	ResourceManager::GetInstance().DeleteFromCSV(kStageDatas[m_nowStage - 1].resources);
	clsDx();
}

void SceneMain::Load()
{
	// 必要なリソースをロード
	auto& resourceManager = ResourceManager::GetInstance();
	resourceManager.LoadFromCSV(kStageDatas[m_nowStage - 1].resources);
}

void SceneMain::Init()
{
	UIManager::GetInstance().Init();

	// CollisionManagerを生成し、ServiceLocatorに登録
	m_pCollisionManager = std::make_unique<CollisionManager>();
	ServiceLocator::GetInstance().ProvideCollisionManager(m_pCollisionManager.get());

	// クリア時の処理をEventManagerに登録
	EventManager::GetInstance().Register(kClearEventName, [this]() { StageClear(); }, shared_from_this());

	// GameObjectManagerを生成
	m_pGameObjectManager = std::make_shared<GameObjectManager>();
	// ステージを生成
	m_pGameObjectManager->Add(std::make_shared<StaticStage>());
	// バリアを生成
	StageGenerator::PlaceBarriers(m_pGameObjectManager, kStageDatas[m_nowStage - 1].barriers);
	// ハックポイントを生成
	auto hackPoint = std::make_shared<HackPoint>();
	hackPoint->SetEvent(L"RemoveBarrier1");
	hackPoint->GetComponent<Transform>()->pos = Vector3(4290, 500, 2300);
	m_pGameObjectManager->Add(hackPoint);
	// プレイヤーを生成
	m_pPlayer = std::make_shared<Player>();
	m_pGameObjectManager->Add(m_pPlayer);
	// カメラを生成(プレイヤーの移動後に追従させるため、プレイヤーより後に追加する)
	auto pCamera = std::make_shared<Camera>();
	m_pGameObjectManager->Add(pCamera);
	// ハッキングマネージャーを生成
	m_pHackingManager = std::make_shared<HackingManager>();
	m_pHackingManager->Init(m_pPlayer);
	// カメラとプレイヤーにお互いの弱参照を渡す
	m_pPlayer->SetCamera(pCamera);
	pCamera->SetPlayer(m_pPlayer);
	// プレイヤーにマネージャーを渡す
	m_pPlayer->SetHackingManager(m_pHackingManager);
	m_pPlayer->SetGameObjectManager(m_pGameObjectManager);
	// 敵を生成
	EnemyFactory::Info enemyFactoryInfo;
	enemyFactoryInfo.pGameObjectManager = m_pGameObjectManager;
	enemyFactoryInfo.pPlayer = m_pPlayer;
	EnemyPlacer::Place(enemyFactoryInfo, kStageDatas[m_nowStage - 1].enemies);
	// BoxTriggerを生成(戦闘エリアの入り口)
	auto boxTrigger = std::make_shared<BoxTrigger>(Vector3(3600, 100, 600), Vector3(2900, 500, 900), m_pPlayer, L"BattleArea1Enter");
	m_pGameObjectManager->Add(boxTrigger);
	// 二つ目のBoxTriggerを生成(ボス部屋の入り口)
	boxTrigger = std::make_shared<BoxTrigger>(Vector3(5600, 600, -100), Vector3(5900, 100, 600), m_pPlayer, L"BossAreaEnter");
	m_pGameObjectManager->Add(boxTrigger);
	// BattleAreaを生成
	BattleArea::Info battleAreaInfo = {
		.startEventName = L"BattleArea1Enter",
		.endEventName = L"BattleArea1Clear",
		.csvFilePath = L"data/stages/stage1/datas/battleAreas/battleArea1EnemyPlaceData.csv",
		.pGameObjectManager = m_pGameObjectManager,
		.pPlayer = m_pPlayer
	};
	auto battleArea = std::make_shared<BattleArea>(battleAreaInfo);
	m_pGameObjectManager->Add(battleArea);
	// 二つ目のBattleAreaを生成
	battleAreaInfo = {
		.startEventName = L"BossAreaEnter",
		.endEventName = L"DefeatedBoss",
		.csvFilePath = L"data/stages/stage1/datas/battleAreas/bossAreaEnemyPlaceData.csv",
		.pGameObjectManager = m_pGameObjectManager,
		.pPlayer = m_pPlayer
	};
	battleArea = std::make_shared<BattleArea>(battleAreaInfo);
	m_pGameObjectManager->Add(battleArea);
}

void SceneMain::Update()
{
	// まだGameObjectManagerが生成されていないなら処理しない
	if (!m_pGameObjectManager) return;

	UIManager::GetInstance().Update();
	m_pCollisionManager->Update();
	m_pGameObjectManager->Update();
	m_pHackingManager->Update();

	// プレイヤーが死んだらゲームオーバー
	if (m_pPlayer->IsDead())
	{
		m_sceneManager.ChangeScene(std::make_shared<SceneResult>(m_sceneManager, false),true);
		return;
	}

	// startボタンでポーズ
	if (PadInput::GetInstance().IsTriggerd(XINPUT_BUTTON_START))
	{
		m_sceneManager.PushScene(std::make_shared<ScenePause>(m_sceneManager));
		return;
	}

#ifdef _DEBUG
	// Qキーでクリア
	if (CheckHitKey(KEY_INPUT_Q))
	{
		m_sceneManager.ChangeScene(std::make_shared<SceneResult>(m_sceneManager, true),true);
		return;
	}
	// Wキーでボス部屋にテレポート
	if (CheckHitKey(KEY_INPUT_W))
	{
		m_pPlayer->GetComponent<Transform>()->pos = Vector3(5700, 100, 200);
	}
#endif
}

void SceneMain::Draw()
{
	// まだGameObjectManagerが作られていないなら描画しない
	if (!m_pGameObjectManager) return;

	m_pGameObjectManager->Draw();

	UIManager::GetInstance().Draw();

	m_pHackingManager->Draw();

	DrawCircle(Game::kScreenWidth / 2, Game::kScreenHeight / 2, 10, 0xffffff, false, 3);

#ifdef _DEBUG
	DrawGrid();

	DrawString(0, 0, L"SceneMain", 0xffffff);
	DrawString(0, 16, L"Qキーでクリアする", 0xffffff);
#endif
}

void SceneMain::StageClear()
{
	m_sceneManager.ChangeScene(std::make_shared<SceneResult>(m_sceneManager, true),true);
}