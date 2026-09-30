#pragma once
#include "SceneBase.h"

class GameObjectManager;
class CollisionManager;
class Camera;
class Player;

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
    std::shared_ptr<Player> m_pPlayer = nullptr;
    std::shared_ptr<Camera> m_pCamera = nullptr;
};

