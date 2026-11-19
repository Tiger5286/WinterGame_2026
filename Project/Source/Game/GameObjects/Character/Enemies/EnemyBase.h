#pragma once
#include "../Character.h"
#include "Components/Hackable.h"

class Player;

class EnemyBase :
    public Character
{
public:
    EnemyBase(std::shared_ptr<Player> pPlayer);
    virtual ~EnemyBase() override;

    virtual void Init() override = 0;
    virtual void Update() override = 0;
    virtual void Draw() override = 0;

    void OnDamage(int damage);
    virtual bool IsDying() = 0;
protected:
    virtual void Death() = 0;
    virtual void OnHacked(Hackable::HackedData data) = 0;
    void OnWasShot(const CollisionManager::ShotInfo& info) override;

protected:
    std::weak_ptr<Player> m_pPlayer;
};