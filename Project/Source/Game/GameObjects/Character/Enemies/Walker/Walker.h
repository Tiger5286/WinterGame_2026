#pragma once
#include "../EnemyBase.h"
#include "Components/Hackable.h"

class Model;
class Graph;
class EnemyHPUI;

class Walker :
    public EnemyBase
{
public:
    static constexpr int kMaxHp = 500;

public:
    Walker(std::shared_ptr<Player> pPlayer);
    ~Walker();

    void Init() override;
    void Update() override;
    void Draw() override;

    void OnWasShot(const CollisionManager::ShotInfo& info) override;

    // 死亡ステート中かどうか
    bool IsDying();

private:
    void OnHacked(Hackable::HackedData data);
    void RotToPlayer();
    void SetHitbox();

private:
    std::unique_ptr<Model> m_pModel;
    Graph* m_pModelTexture[2] = { nullptr,nullptr };    // 0 : 通常テクスチャ / 1 : ハックされたテクスチャ
    int m_attackCooltime = 0;
    std::shared_ptr<EnemyHPUI> m_pHPUI = nullptr;

    friend class WalkerState;
};

