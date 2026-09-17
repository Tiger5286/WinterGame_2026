#include "SceneMain.h"
#include "DxLib.h"

SceneMain::SceneMain(SceneManager& sceneManager):
	SceneBase(sceneManager)
{
}

SceneMain::~SceneMain()
{
}

void SceneMain::Init()
{
}

void SceneMain::Update()
{
}

void SceneMain::Draw() const
{

	DrawGrid();

#ifdef _DEBUG
	DrawString(0, 0, L"SceneMain", 0xffffff);
#endif
}
