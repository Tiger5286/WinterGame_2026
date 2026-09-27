#pragma once
#include "SceneBase.h"

class GameObjectManager;
class CollisionManager;

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
    std::unique_ptr<CollisionManager> m_pCollisionManager;
    std::shared_ptr<GameObjectManager> m_pGameObjectManager = nullptr;
};

