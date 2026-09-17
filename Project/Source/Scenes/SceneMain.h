#pragma once
#include "SceneBase.h"
class SceneMain :
    public SceneBase
{
public:
    SceneMain(SceneManager& sceneManager);
    virtual ~SceneMain() override;

    void Init() override;
    void Update() override;
    void Draw() const override;

private:

};

