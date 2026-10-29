#pragma once
#include "../EnemyBase.h"
#include "Components/Hackable.h"

class Model;
class EnemyHPUI;

class Mutant :
    public EnemyBase
{
public:
    static constexpr int kMaxHp = 3000;

public:
    Mutant(std::shared_ptr<Player> pPlayer);
    ~Mutant();

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
    int m_diffuseTexIndex = 0;  // ハッキング時に差し替えるテクスチャの番号
    int m_attackCooltime = 0;
    std::shared_ptr<EnemyHPUI> m_pHPUI = nullptr;

    friend class MutantState;
};

