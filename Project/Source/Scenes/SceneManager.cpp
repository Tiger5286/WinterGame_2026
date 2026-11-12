#include "SceneManager.h"
#include "DxLib.h"
#include "SceneBase.h"

void SceneManager::ChangeScene(std::shared_ptr<SceneBase> pNewScene, bool isFade)
{
	m_pNextScene = pNewScene;
	m_nextChangeScene = NextChangeScene::Change;
}

void SceneManager::PushScene(std::shared_ptr<SceneBase> pNewScene,bool isFade)
{
	m_pNextScene = pNewScene;
	m_nextChangeScene = NextChangeScene::Push;
}

void SceneManager::PopScene(bool isFade)
{
	m_nextChangeScene = NextChangeScene::Pop;
}

void SceneManager::ResetScene(std::shared_ptr<SceneBase> pNewScene, bool isFade)
{
	m_pNextScene = pNewScene;
	m_nextChangeScene = NextChangeScene::Reset;
}

void SceneManager::Update()
{
	if (!m_pScenes.empty())
	{
		m_pScenes.back()->Update();
	}

	if (m_nextChangeScene != NextChangeScene::None)
	{
		switch (m_nextChangeScene)
		{
		case NextChangeScene::Change:
			// シーンが空なら新しいシーンを追加する
			if (m_pScenes.empty())
			{
				m_pScenes.push_back(m_pNextScene);
			}
			else	// シーンが空でないなら最後のシーンを新しいシーンにする
			{
				m_pScenes.back() = m_pNextScene;
			}
			break;
		case NextChangeScene::Reset:
			m_pScenes.clear();
			m_pScenes.push_back(m_pNextScene);
			break;
		case NextChangeScene::Push:
			m_pScenes.push_back(m_pNextScene);
			break;
		case NextChangeScene::Pop:
			m_pScenes.pop_back();
			break;
		}

		if (m_pNextScene)
		{
			m_pNextScene->Init();
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