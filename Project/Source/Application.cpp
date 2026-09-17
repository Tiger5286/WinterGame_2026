#include "Application.h"
#include "Dxlib.h"
#include "EffekseerForDXLib.h"
#include "Game.h"
#include "Scenes/SceneManager.h"
#include "Scenes/SceneMain.h"

#include <memory>
#include <string>

Application& Application::GetInstance()
{
    static Application instance;
    return instance;
}

bool Application::Init()
{
	ChangeWindowMode(false);	// フルスクリーンで起動
#ifdef _DEBUG
	ChangeWindowMode(true); // Debugならウィンドウモードで起動
#endif
	SetMainWindowText(L"Hack Bot"); // ウインドウのタイトルを設定
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, 32); // 画面サイズと色数を設定

	// DirectX11を使用するようにする。(DirectX9も可、一部機能不可)
	// Effekseerを使用するには必ず設定する。
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return false;
	}

	// Effekseerを初期化する。
	// 引数には画面に表示する最大パーティクル数を設定する。
	if (Effekseer_Init(8000) == -1)
	{
		return false;
	}
	// フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ。
	// Effekseerを使用する場合は必ず設定する。
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
	// DXライブラリのデバイスロストした時のコールバックを設定する。
	// ウインドウとフルスクリーンの切り替えが発生する場合は必ず実行する。
	// ただし、DirectX11を使用する場合は実行する必要はない。
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
	// エフェクトの歪みを有効にする
	Effekseer_InitDistortion();

	return true;
}

void Application::Run()
{
	// 描画対象をバックバッファに変更
	SetDrawScreen(DX_SCREEN_BACK);

	// カリングの設定
	SetUseBackCulling(true);

	// Zバッファの設定
	SetUseZBuffer3D(true);	// Zバッファを使います
	SetWriteZBuffer3D(true);	// 描画する物体はZバッファにも距離を書き込む

	// ライティングの設定
	SetUseLighting(true);

	SceneManager sceneManager;
	sceneManager.ChangeScene(std::make_shared<SceneMain>(sceneManager));

	while (ProcessMessage() != -1 && !m_isRequestExit)
	{
		auto start = GetNowHiPerformanceCount(); // フレーム開始時間を取得
		ClearDrawScreen(); // 画面をクリア


		sceneManager.Update();
		sceneManager.Draw();

		// escキーで終了
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			RequestExit();
		}

		ScreenFlip(); // 描画した内容を画面に反映する
		while (GetNowHiPerformanceCount() - start < 16667) {} // 約16.667ミリ秒(1/60秒)待つことで60FPSに固定
	}
}

void Application::Terminate()
{
	Effkseer_End();	// Effekseerの終了処理
	DxLib_End();	// ＤＸライブラリの終了処理
	return;
}