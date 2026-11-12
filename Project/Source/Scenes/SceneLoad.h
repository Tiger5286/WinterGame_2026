#pragma once
#include "SceneBase.h"
class SceneLoad :
    public SceneBase
{
public:
    SceneLoad(SceneManager& sceneManager);
    ~SceneLoad();

    void Init() override;
    void Update() override;
    void Draw() const override;

private:
    const int m_firstLoadNum;

    // ロードが終わってからのフレームをカウントする
    int m_loadEndFrame = 0;
};

