#pragma once
#include "../Character.h"
#include "Components/Hackable.h"
#include "Game/SpawnContext.h"

class Player;
class GameObjectManager;

class EnemyBase :
    public Character
{
public:
    EnemyBase(const SpawnContext::Info& info);
    virtual ~EnemyBase() override;

    virtual void Init() override = 0;
    virtual void Update() override = 0;
    virtual void Draw() override = 0;

    void OnDamage(int damage);
    virtual bool IsDying() = 0;
protected:
    virtual void Death() = 0;
    virtual void OnHacked(Hackable::HackedData data);
    void OnWasShot(const CollisionManager::ShotInfo& info) override;
    void DropSpecialNode();

protected:
    std::weak_ptr<Player> m_pPlayer;
    std::weak_ptr<GameObjectManager> m_pGameObjectManager;
};