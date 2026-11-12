#include "SceneLoad.h"
#include "DxLib.h"
#include "SceneManager.h"

namespace
{
	constexpr int kMinLoadFrame = 30;
}

SceneLoad::SceneLoad(SceneManager& sceneManager) :
	SceneBase(sceneManager),
	m_firstLoadNum(GetASyncLoadNum())
{
}

SceneLoad::~SceneLoad()
{
	// ロードシーンが終わるとき非同期読み込みを終了する
	SetUseASyncLoadFlag(false);
}

void SceneLoad::Init()
{

}

void SceneLoad::Update()
{
	// ロードしているリソースの数を取得
	int loadNum = GetASyncLoadNum();
	// ロードが終わったらフレームをカウント
	if (loadNum == 0)
	{
		m_loadEndFrame++;
	}
	// ロードが終わって一定時間たったらロードシーンをポップ
	if (m_loadEndFrame > kMinLoadFrame)
	{
		m_sceneManager.PopScene(true);
		return;
	}
}

void SceneLoad::Draw() const
{
	int loadNum = GetASyncLoadNum();
#ifdef _DEBUG
	DrawString(0, 0, L"SceneLoad", 0xffffff);
	DrawFormatString(0, 16, 0xffffff, L"%d / %d", m_firstLoadNum - loadNum, m_firstLoadNum);
#endif
}