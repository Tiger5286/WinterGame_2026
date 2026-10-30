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
#include "Game/GameObjects/Character/Enemies/Walker/Walker.h"
#include "Game/GameObjects/Character/Enemies/Mutant/Mutant.h"
#include "SceneResult.h"
#include "SceneManager.h"
#include "System/PadInput.h"

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
		{ L"data/models/stage/stage1/Stage1StaticCol.mv1", L"StaticStageModel" },
		{ L"data/models/stage/stage1/Barrier1.mv1",L"Barrier1"},
		{ L"data/models/enemies/Walker.mv1", L"Walker" },
		{ L"data/models/enemies/Mutant.mv1", L"Mutant"}
	};

	const LoadData kLoadGraphFiles[] = {
		{ L"data/models/enemies/Walker.fbm/Ch36_1001_Diffuse_Cyber.png", L"NormalWalkerTex" },
		{ L"data/models/enemies/Walker.fbm/Ch36_1001_Diffuse_Cyber_Open.png", L"HackedWalkerTex" },
		{ L"data/models/enemies/Mutant.fbm/Mutant_diffuse.png", L"NormalMutantTex" },
		{ L"data/models/enemies/Mutant.fbm/Mutant_diffuse_open.png", L"HackedMutantTex" },
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
	// SceneMainでしか使わないリソースを削除
	for (auto& model : kLoadModelFiles)
	{
		ResourceManager::GetInstance().Delete(model.key);
	}
	for (auto& graph : kLoadGraphFiles)
	{
		ResourceManager::GetInstance().Delete(graph.key);
	}
	clsDx();
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
	for (const auto& file : kLoadGraphFiles)
	{
		resourceManager.LoadGraph(file.filePath, file.key);
	}
	// GameObjectManagerを生成
	m_pGameObjectManager = std::make_shared<GameObjectManager>();
	// ステージを生成
	m_pGameObjectManager->Add(std::make_shared<StaticStage>());
	m_pGameObjectManager->Add(std::make_shared<Barrier>());
	auto hackPoint = std::make_shared<HackPoint>();
	hackPoint->SetEvent(L"RemoveBarrier1");
	hackPoint->GetComponent<Transform>()->pos = Vector3(4300, 500, 2300);
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
	m_pBoss = std::make_shared<Mutant>(m_pPlayer);
	m_pBoss->GetComponent<Transform>()->pos = Vector3(0, 100, 300);
	m_pGameObjectManager->Add(m_pBoss);
	auto pEnemy = std::make_shared<Walker>(m_pPlayer);
	pEnemy->GetComponent<Transform>()->pos = Vector3(0, 100, 1000);
	m_pGameObjectManager->Add(pEnemy);
}

void SceneMain::Update()
{
	UIManager::GetInstance().Update();
	m_pCollisionManager->Update();
	m_pGameObjectManager->Update();
	m_pHackingManager->Update();

	// プレイヤーが死んだらゲームオーバー
	if (m_pPlayer->IsDead())
	{
		m_sceneManager.ChangeSceneWithFade(std::make_shared<SceneResult>(m_sceneManager, false));
		return;
	}
	// ボスが死んだらクリア
	if (m_pBoss != nullptr)
	{
		if (m_pBoss->IsDead())
		{
			m_sceneManager.ChangeSceneWithFade(std::make_shared<SceneResult>(m_sceneManager, true));
			return;
		}
	}

#ifdef _DEBUG
	// Qキーでクリア
	if (CheckHitKey(KEY_INPUT_Q))
	{
		m_sceneManager.ChangeSceneWithFade(std::make_shared<SceneResult>(m_sceneManager, true));
		return;
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
