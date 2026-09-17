#include "DxLib.h"
#include "Application.h"

// プログラムは WinMain から始まる
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// アプリケーションのインスタンスを生成
	auto& app = Application::GetInstance();

	// アプリケーションを初期化
	if (!app.Init())
	{
		// 初期化に失敗したら終了
		return -1;
	}

	// 実行
	app.Run();

	// 終了
	app.Terminate();

	return 0;
}