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
	/// シーンを変更する。重なっている場合は、最も上のシーンのみ変更する。
	/// </summary>
	/// <param name="pNewScene">新しいシーン</param>
	/// <param name="isFade">フェードするかどうか</param>
	/// <param name="isASyncLoad">ロード画面を挟むかどうか</param>
	void ChangeScene(std::shared_ptr<SceneBase> pNewScene,bool isFade, bool isASyncLoad = false);

	/// <summary>
	/// シーンを追加する。追加したシーンは最後のシーンの上に乗る。
	/// </summary>
	/// <param name="pNewScene">新しいシーン</param>
	/// <param name="isFade">フェードするかどうか</param>
	/// <param name="isASyncLoad">ロード画面を挟むかどうか</param>
	void PushScene(std::shared_ptr<SceneBase> pNewScene,bool isFade = false, bool isASyncLoad = false);

	/// <summary>
	/// 一番上のシーンを削除する。
	/// </summary>
	/// <param name="isFade">フェードするかどうか</param>
	void PopScene(bool isFade = false);

	/// <summary>
	/// シーンをリセットする。積まれたシーンはすべて消され、新しいシーンだけが残る。
	/// </summary>
	/// <param name="pNewScene">新しいシーン</param>
	/// <param name="isFade">フェードするかどうか</param>
	/// <param name="isASyncLoad">ロード画面を挟むかどうか</param>
	void ResetScene(std::shared_ptr<SceneBase> pNewScene, bool isFade, bool isASyncLoad = false);

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
	bool m_isNextSceneASyncLoad = false;
};

