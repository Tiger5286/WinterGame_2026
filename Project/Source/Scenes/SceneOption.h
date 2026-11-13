#pragma once
#include "SceneBase.h"
class SceneOption :
    public SceneBase
{
public:
    SceneOption(SceneManager& sceneManager);
    ~SceneOption();

    void Init() override;
    void Update() override;
    void Draw() override;

private:

};

