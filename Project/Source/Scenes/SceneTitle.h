#pragma once
#include "SceneBase.h"
class SceneTitle :
    public SceneBase
{
public:
    SceneTitle(SceneManager& sceneManager);
    ~SceneTitle();

    void Init() override;
    void Update() override;
    void Draw() const override;

private:

};

