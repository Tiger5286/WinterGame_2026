#pragma once
#include "SceneBase.h"
class SceneLoad :
    public SceneBase
{
public:
    SceneLoad(SceneManager& sceneManager,std::shared_ptr<SceneBase> pNextScene);
    ~SceneLoad();

    void Init() override;
    void Update() override;
    void Draw() override;

private:
    int m_firstLoadNum = 0;

    // ロードが終わってからのフレームをカウントする
    int m_loadEndFrame = 0;

    bool m_isInitNextScene = false;

    // ロードが終わった後に始まるシーン
    std::shared_ptr<SceneBase> m_pNextScene;
};