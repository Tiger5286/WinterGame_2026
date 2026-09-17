#include "SceneManager.h"
#include "DxLib.h"
#include "SceneBase.h"

void SceneManager::ChangeScene(std::shared_ptr<SceneBase> pNewScene)
{
	// シーンが空なら新しいシーンを追加する
	if (m_pScenes.empty())
	{
		m_pScenes.push_back(pNewScene);
	}
	else	// シーンが空でないなら最後のシーンを新しいシーンにする
	{
		m_pScenes.back() = pNewScene;
	}

	// 新しいシーンの初期化
	pNewScene->Init();
}

void SceneManager::ChangeSceneWithFade(std::shared_ptr<SceneBase> pNewScene)
{
	m_pNextScene = pNewScene;
	m_nextChangeScene = NextChangeScene::Change;
}

void SceneManager::PushScene(std::shared_ptr<SceneBase> pNewScene)
{
	m_pScenes.push_back(pNewScene);
	pNewScene->Init();
}

void SceneManager::PopScene()
{
	if (m_pScenes.size() > 1)
	{
		m_pScenes.pop_back();
	}
}

void SceneManager::PopSceneWithFade()
{
	m_nextChangeScene = NextChangeScene::Pop;
}

void SceneManager::ResetScene(std::shared_ptr<SceneBase> pNewScene)
{
	m_pScenes.clear();
	m_pScenes.push_back(pNewScene);
	pNewScene->Init();
}

void SceneManager::ResetSceneWithFade(std::shared_ptr<SceneBase> pNewScene)
{
	m_pNextScene = pNewScene;
	m_nextChangeScene = NextChangeScene::Reset;
}

void SceneManager::Update()
{
	m_pScenes.back()->Update();

	if (m_nextChangeScene != NextChangeScene::None)
	{
		switch (m_nextChangeScene)
		{
		case NextChangeScene::Change:
			ChangeScene(m_pNextScene);
			break;
		case NextChangeScene::Reset:
			ResetScene(m_pNextScene);
			break;
		case NextChangeScene::Pop:
			PopScene();
			break;
		}

		m_pNextScene = nullptr;
		m_nextChangeScene = NextChangeScene::None;
	}
}

void SceneManager::Draw()
{
	for (auto& scene : m_pScenes)
	{
		scene->Draw();
	}
}

void SceneManager::End()
{
	m_pScenes.clear();
}
