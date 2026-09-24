#include "SceneMain.h"
#include "DxLib.h"
#include "Game/GameObjectManager.h"
#include "Game/GameObjects/Camera/Camera.h"
#include <string>
#include "Resource/ResourceManager.h"
#include "Game/GameObjects/Player/Player.h"

namespace
{
	struct LoadData
	{
		const std::wstring filePath;
		const std::wstring key;
	};

	const LoadData kLoadModelFiles[] = {
		{ L"data/models/player/player.mv1", L"PlayerModel" }
	};
}

SceneMain::SceneMain(SceneManager& sceneManager):
	SceneBase(sceneManager)
{
}

SceneMain::~SceneMain()
{
}

void SceneMain::Init()
{
	// 必要なリソースをロード
	auto& resourceManager = ResourceManager::GetInstance();
	for (const auto& file : kLoadModelFiles)
	{
		resourceManager.LoadModel(file.filePath, file.key);
	}
	// GameObjectManagerを生成
	m_pGameObjectManager = std::make_shared<GameObjectManager>();
	// カメラを生成
	m_pGameObjectManager->Add(std::make_shared<Camera>());
	// プレイヤーを生成
	m_pGameObjectManager->Add(std::make_shared<Player>());
}

void SceneMain::Update()
{
	m_pGameObjectManager->Update();
}

void SceneMain::Draw() const
{
	m_pGameObjectManager->Draw();

#ifdef _DEBUG
	DrawGrid();

	DrawString(0, 0, L"SceneMain", 0xffffff);
#endif
}
