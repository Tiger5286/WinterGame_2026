#pragma once
#include "../GameObject.h"

class Player;

class EnemyBase :
    public GameObject
{
public:
    EnemyBase(std::shared_ptr<Player> pPlayer);
    virtual ~EnemyBase() override;

    virtual void Init() override = 0;
    virtual void Update() override = 0;
    virtual void Draw() override = 0;

protected:
    std::weak_ptr<Player> m_pPlayer;
};

