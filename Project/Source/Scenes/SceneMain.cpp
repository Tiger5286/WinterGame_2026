#include "SceneMain.h"
#include "DxLib.h"
#include <string>
#include "Game.h"
#include "Game/GameObjectManager.h"
#include "Game/GameObjects/Camera/Camera.h"
#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Stage/StaticStage.h"
#include "Game/CollisionManager.h"
#include "Resource/ResourceManager.h"
#include "System/ServiceLocator.h"
#include "Components/Component.h"
#include "Components/Transform.h"

namespace
{
	struct LoadData
	{
		const std::wstring filePath;
		const std::wstring key;
	};

	const LoadData kLoadModelFiles[] = {
		{ L"data/models/player/player.mv1", L"PlayerModel" },
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
	m_pCamera = std::make_shared<Camera>();
	m_pGameObjectManager->Add(m_pCamera);
	// プレイヤーを生成
	m_pPlayer = std::make_shared<Player>();
	m_pGameObjectManager->Add(m_pPlayer);
	// カメラとプレイヤーにお互いの弱参照を渡す
	m_pPlayer->SetCamera(m_pCamera);
	m_pCamera->SetPlayer(m_pPlayer);
}

void SceneMain::Update()
{
	m_pCollisionManager->Update();
	m_pGameObjectManager->Update();

	// ライトの方向をカメラ→プレイヤーにする
	Vector3 cameraToPlayer = m_pPlayer->GetComponent<Transform>()->pos - m_pCamera->GetComponent<Transform>()->pos;
	SetLightDirection(cameraToPlayer);
}

void SceneMain::Draw() const
{
	m_pGameObjectManager->Draw();

	DrawCircle(Game::kScreenWidth / 2, Game::kScreenHeight / 2, 10, 0xffffff, false, 3);

#ifdef _DEBUG
	DrawGrid();

	DrawString(0, 0, L"SceneMain", 0xffffff);
#endif
}
