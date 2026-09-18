#include "SceneMain.h"
#include "DxLib.h"
#include "Game/GameObjectManager.h"
#include "Game/GameObjects/Camera/Camera.h"

SceneMain::SceneMain(SceneManager& sceneManager):
	SceneBase(sceneManager)
{
}

SceneMain::~SceneMain()
{
}

void SceneMain::Init()
{
	m_pGameObjectManager = std::make_shared<GameObjectManager>();

	m_pGameObjectManager->Add(std::make_shared<Camera>());
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
