#pragma once
#include "SceneBase.h"
#include "System/Menu.h"

class ScenePause :
    public SceneBase
{
public:
    ScenePause(SceneManager& sceneManager);
    ~ScenePause();

    void Init() override;
    void Update() override;
    void Draw() const override;

private:
    Menu m_menu;
};

