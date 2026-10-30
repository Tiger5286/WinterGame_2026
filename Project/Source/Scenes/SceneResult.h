#pragma once
#include "SceneBase.h"
class SceneResult :
    public SceneBase
{
public:
    SceneResult(SceneManager& sceneManager, bool isClear);
    ~SceneResult();

    void Init() override;
    void Update() override;
    void Draw() const override;

private:
    bool m_isClear;
};

