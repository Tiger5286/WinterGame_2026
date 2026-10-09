#include "SceneMain.h"
#include "DxLib.h"
#include <string>
#include "Game.h"
#include "Game/GameObjectManager.h"
#include "Game/GameObjects/Camera/Camera.h"
#include "Game/GameObjects/Character/Player/Player.h"
#include "Game/GameObjects/Character/Enemies/TestEnemy.h"
#include "Game/GameObjects/Stage/StaticStage.h"
#include "Game/CollisionManager.h"
#include "Resource/ResourceManager.h"
#include "System/ServiceLocator.h"
#include "Components/Component.h"
#include "Components/Transform.h"
#include "Game/HackingManager.h"
#include "System/UIManager.h"
#include "Game/GameObjects/Character/Enemies/Walker/Walker.h"

namespace
{
	struct LoadData
	{
		const std::wstring filePath;
		const std::wstring key;
	};

	const LoadData kLoadModelFiles[] = {
		{ L"data/models/player/player.mv1", L"PlayerModel" },
		{ L"data/models/gun/gun.mv1",L"GunModel" },
		{ L"data/models/enemies/testEnemy.mv1", L"TestEnemyModel"},
		{ L"data/models/stage/test/StaticStageCol.mv1", L"TestStageModel" }
	};
}

SceneMain::SceneMain(SceneManager& sceneManager):
	SceneBase(sceneManager)
{
}

SceneMain::~SceneMain()
{
	// CollisionManagerが消えるのでServiceLocatorの登録を解除
	ServiceLocator::GetInstance().ProvideCollisionManager(nullptr);
}

void SceneMain::Init()
{
	UIManager::GetInstance().Init();

	// CollisionManagerを生成し、ServiceLocatorに登録
	m_pCollisionManager = std::make_unique<CollisionManager>();
	ServiceLocator::GetInstance().ProvideCollisionManager(m_pCollisionManager.get());

	// 必要なリソースをロード
	auto& resourceManager = ResourceManager::GetInstance();
	for (const auto& file : kLoadModelFiles)
	{
		resourceManager.LoadModel(file.filePath, file.key);
	}
	// GameObjectManagerを生成
	m_pGameObjectManager = std::make_shared<GameObjectManager>();
	// ステージを生成
	m_pGameObjectManager->Add(std::make_shared<StaticStage>());
	// カメラを生成
	auto pCamera = std::make_shared<Camera>();
	m_pGameObjectManager->Add(pCamera);
	// プレイヤーを生成
	auto pPlayer = std::make_shared<Player>();
	m_pGameObjectManager->Add(pPlayer);
	// ハッキングマネージャーを生成
	m_pHackingManager = std::make_shared<HackingManager>();
	m_pHackingManager->Init(pPlayer);
	// カメラとプレイヤーにお互いの弱参照を渡す
	pPlayer->SetCamera(pCamera);
	pCamera->SetPlayer(pPlayer);
	// プレイヤーにマネージャーを渡す
	pPlayer->SetHackingManager(m_pHackingManager);
	pPlayer->SetGameObjectManager(m_pGameObjectManager);
	// 敵を生成
	std::shared_ptr<GameObject> pEnemy = std::make_shared<Walker>(pPlayer);
	pEnemy->GetComponent<Transform>()->pos = Vector3(0, 0, 300);
	m_pGameObjectManager->Add(pEnemy);
	pEnemy = std::make_shared<TestEnemy>(pPlayer);
	pEnemy->GetComponent<Transform>()->pos = Vector3(200, 0, 200);
	m_pGameObjectManager->Add(pEnemy);
	pEnemy = std::make_shared<TestEnemy>(pPlayer);
	pEnemy->GetComponent<Transform>()->pos = Vector3(-200, 0, 200);
	m_pGameObjectManager->Add(pEnemy);
}

void SceneMain::Update()
{
	UIManager::GetInstance().Update();
	m_pCollisionManager->Update();
	m_pGameObjectManager->Update();
	m_pHackingManager->Update();
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
#endif
}
