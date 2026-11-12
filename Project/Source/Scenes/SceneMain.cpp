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
#include "Game/BarrierPlacer.h"
#include "Game/GameObjects/BattleArea.h"
#include "System/EventManager.h"

namespace
{
	const std::wstring kResourceFilePath = L"data/loadData.csv";
}

SceneMain::SceneMain(SceneManager& sceneManager):
	SceneBase(sceneManager)
{
}

SceneMain::~SceneMain()
{
	// CollisionManagerが消えるのでServiceLocatorの登録を解除
	ServiceLocator::GetInstance().ProvideCollisionManager(nullptr);
	// SceneMainでしか使わないリソースを削除
	ResourceManager::GetInstance().DeleteFromCSV(kResourceFilePath);
	clsDx();
}

void SceneMain::Init()
{
	UIManager::GetInstance().Init();

	// CollisionManagerを生成し、ServiceLocatorに登録
	m_pCollisionManager = std::make_unique<CollisionManager>();
	ServiceLocator::GetInstance().ProvideCollisionManager(m_pCollisionManager.get());

	// クリア時の処理をEventManagerに登録
	EventManager::GetInstance().Register(L"DefeatedBoss", [this]() { StageClear(); }, shared_from_this());

	// 必要なリソースをロード
	auto& resourceManager = ResourceManager::GetInstance();
	resourceManager.LoadFromCSV(kResourceFilePath);
	// GameObjectManagerを生成
	m_pGameObjectManager = std::make_shared<GameObjectManager>();
	// ステージを生成
	m_pGameObjectManager->Add(std::make_shared<StaticStage>());
	// バリアを生成
	BarrierPlacer::Place(m_pGameObjectManager, L"data/stages/stage1/barrierData.csv");
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
	EnemyPlacer::Place(enemyFactoryInfo, L"data/stages/stage1/enemyPlaceData.csv");
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
		.csvFilePath = L"data/stages/stage1/battleArea1EnemyPlaceData.csv",
		.pGameObjectManager = m_pGameObjectManager,
		.pPlayer = m_pPlayer
	};
	auto battleArea = std::make_shared<BattleArea>(battleAreaInfo);
	m_pGameObjectManager->Add(battleArea);
	// 二つ目のBattleAreaを生成
	battleAreaInfo = {
		.startEventName = L"BossAreaEnter",
		.endEventName = L"DefeatedBoss",
		.csvFilePath = L"data/stages/stage1/BossAreaEnemyPlaceData.csv",
		.pGameObjectManager = m_pGameObjectManager,
		.pPlayer = m_pPlayer
	};
	battleArea = std::make_shared<BattleArea>(battleAreaInfo);
	m_pGameObjectManager->Add(battleArea);
}

void SceneMain::Update()
{
	UIManager::GetInstance().Update();
	m_pCollisionManager->Update();
	m_pGameObjectManager->Update();
	m_pHackingManager->Update();

	// プレイヤーが死んだらゲームオーバー
	if (m_pPlayer->IsDead() && !m_isDeadPlayer)
	{
		m_isDeadPlayer = true;
		m_sceneManager.ChangeScene(std::make_shared<SceneResult>(m_sceneManager, false),true);
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

void SceneMain::Draw() const
{
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