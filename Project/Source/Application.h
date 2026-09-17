#pragma once

class SceneManager;

class Application
{
public:
	// インスタンスを取得する
	static Application& GetInstance();
	virtual ~Application() = default;

	// コピーコンストラクタと代入演算子を禁止
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
private:
	// コンストラクタをprivateに
	Application() = default;
public:
	// 初期化処理
	bool Init();
	// 実行処理
	void Run();
	// 終了処理
	void Terminate();

	// 終了を依頼する
	void RequestExit() { m_isRequestExit = true; }

private:
	bool m_isRequestExit = false;
};

