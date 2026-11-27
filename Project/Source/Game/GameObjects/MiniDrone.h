#pragma once
#include "GameObject.h"
#include <memory>

class Model;
class Player;
class HackingManager;
class SimpleAnimator;
class Transform;

class MiniDrone :
    public GameObject
{
public:
    MiniDrone(const std::shared_ptr<Player>& pPlayer, const std::shared_ptr<HackingManager>& pHackingManager);
    ~MiniDrone();

    void Init() override;
    void Update() override;
    void Draw() override;

private:
    std::weak_ptr<Player> m_pPlayer;
    std::weak_ptr<HackingManager> m_pHackingManager;
    std::unique_ptr<Model> m_pModel;
    SimpleAnimator* m_animator = nullptr;
    Transform* m_playerTrans = nullptr;
    Vector3 m_pos;
};