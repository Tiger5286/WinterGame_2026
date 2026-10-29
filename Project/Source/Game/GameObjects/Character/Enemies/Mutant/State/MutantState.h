#pragma once
#include "Components/State/State.h"
#include "Utility/Vector3.h"
#include <memory>

class Mutant;
class Player;

class MutantState :
    public State<Mutant>
{
public:
    static constexpr float kAccel = 0.2f;

    // この距離より近づいたら通常攻撃する
    static constexpr float kAttackDist = 350.0f;

public:
    enum class ID
    {
        Approach,
        Attack,
        Interval,
        Death,

        Num
    };

public:
    MutantState(Mutant& mutant);
    virtual ~MutantState();

    virtual void Enter() override = 0;
    virtual void Update() override = 0;
    virtual void Exit() override = 0;

    virtual ID GetID() const = 0;

protected:
    std::shared_ptr<Player> GetPlayer() const;
    int GetAttackCooltime() const;
    void SetAttackCooltime(int coolTime);
    void Die();
    void RotToPlayer();

    // プレイヤーとの水平方向のベクトル
    Vector3 GetToPlayerXZ() const;
};

