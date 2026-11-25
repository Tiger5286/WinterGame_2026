#pragma once
#include "../EnemyBase.h"
#include "Components/Hackable.h"
#include "Game/SpawnContext.h"

class Model;
class Graph;
class EnemyHPUI;

class Walker :
    public EnemyBase
{
public:
    static constexpr int kMaxHp = 500;

public:
    Walker(const SpawnContext::Info& info);
    ~Walker();

    void Init() override;
    void Update() override;
    void Draw() override;

    // 死亡ステート中かどうか
    bool IsDying() override;

private:
    void Death() override;
    void OnHacked(Hackable::HackedData data) override;

    void RotToPlayer();
    void SetHitbox();

private:
    std::unique_ptr<Model> m_pModel;
    Graph* m_pModelTexture[2] = { nullptr,nullptr };    // 0 : 通常テクスチャ / 1 : ハックされたテクスチャ
    float m_attackCooltime = 0.0f;
    std::shared_ptr<EnemyHPUI> m_pHPUI = nullptr;

    friend class WalkerState;
};