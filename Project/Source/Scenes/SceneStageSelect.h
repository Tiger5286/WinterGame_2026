#pragma once
#include "SceneBase.h"
class SceneStageSelect :
    public SceneBase
{
public:
    SceneStageSelect(SceneManager& sceneManager);
    ~SceneStageSelect();

    void Init() override;
    void Update() override;
    void Draw() override;

private:

};

