#pragma once
#include <list>
#include <memory>

class SceneBase;

class SceneManager
{
public:
	SceneManager() = default;
	~SceneManager() = default;

	/// <summary>
	/// 一番上のシーンを変更する
	/// </summary>
	/// <param name="pNewScene">新しいシーン</param>
	/// <param name="isCallLoadScene">ロードシーンを使うかどうか</param>
	void ChangeScene(std::shared_ptr<SceneBase> pNewScene);
	void ChangeSceneWithFade(std::shared_ptr<SceneBase> pNewScene);

	/// <summary>
	/// シーンを追加する
	/// </summary>
	/// <param name="pNewScene">新しいシーン</param>
	void PushScene(std::shared_ptr<SceneBase> pNewScene);

	/// <summary>
	/// 一番上のシーンを削除する
	/// </summary>
	void PopScene();
	void PopSceneWithFade();

	/// <summary>
	/// シーンをリセットする
	/// </summary>
	/// <param name="pNewScene">新しいシーン</param>
	void ResetScene(std::shared_ptr<SceneBase> pNewScene);
	void ResetSceneWithFade(std::shared_ptr<SceneBase> pNewScene);

	void Update();
	void Draw();
	void End();

private:
	enum class NextChangeScene
	{
		None,
		Change,
		Reset,
		Pop
	};

	std::list<std::shared_ptr<SceneBase>> m_pScenes;
	std::shared_ptr<SceneBase> m_pNextScene = nullptr;
	NextChangeScene m_nextChangeScene = NextChangeScene::None;
};

