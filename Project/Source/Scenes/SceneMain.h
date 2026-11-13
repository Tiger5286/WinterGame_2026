#pragma once
#include "SceneBase.h"

class GameObjectManager;
class CollisionManager;
class HackingManager;
class Camera;
class Player;

class SceneMain :
    public SceneBase
{
public:
    SceneMain(SceneManager& sceneManager);
    virtual ~SceneMain() override;

    void Load() override;

    void Init() override;
    void Update() override;
    void Draw() override;

private:
	void StageClear();

private:
    std::unique_ptr<CollisionManager> m_pCollisionManager;
    std::shared_ptr<GameObjectManager> m_pGameObjectManager = nullptr;
    std::shared_ptr<HackingManager> m_pHackingManager = nullptr;

    std::shared_ptr<Player> m_pPlayer = nullptr;
};

