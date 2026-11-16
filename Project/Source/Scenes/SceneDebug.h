#pragma once
#include "SceneBase.h"
class SceneDebug :
    public SceneBase
{
public:
    SceneDebug(SceneManager& sceneManager);
    ~SceneDebug();

    void Init() override;
    void Update() override;
    void Draw() override;

private:
    int m_selectIndex = 0;
};

