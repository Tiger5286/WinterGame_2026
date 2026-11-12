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
	void ChangeScene(std::shared_ptr<SceneBase> pNewScene,bool isFade = false);

	/// <summary>
	/// シーンを追加する
	/// </summary>
	/// <param name="pNewScene">新しいシーン</param>
	void PushScene(std::shared_ptr<SceneBase> pNewScene,bool isFade = false);

	/// <summary>
	/// 一番上のシーンを削除する
	/// </summary>
	void PopScene(bool isFade = false);

	/// <summary>
	/// シーンをリセットする
	/// </summary>
	/// <param name="pNewScene">新しいシーン</param>
	void ResetScene(std::shared_ptr<SceneBase> pNewScene, bool isFade = false);

	void Update();
	void Draw();
	void End();

private:
	enum class NextChangeScene
	{
		None,
		Change,
		Reset,
		Push,
		Pop
	};

	std::list<std::shared_ptr<SceneBase>> m_pScenes;
	std::shared_ptr<SceneBase> m_pNextScene = nullptr;
	NextChangeScene m_nextChangeScene = NextChangeScene::None;
};

