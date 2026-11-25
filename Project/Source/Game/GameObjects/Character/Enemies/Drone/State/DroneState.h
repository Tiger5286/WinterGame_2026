#pragma once
#include "Components/State/State.h"
#include "Utility/Vector3.h"
#include <memory>

class Drone;
class Player;

class DroneState :
    public State<Drone>
{
public:
    enum class ID
    {
        Idle,
        Shoot,
        Fall,

        Num
    };

public:
    DroneState(Drone& drone);
    virtual ~DroneState();

    virtual void Enter() override = 0;
    virtual void Update() override = 0;
    virtual void Exit() override = 0;

    virtual ID GetID() const = 0;

protected:
    std::shared_ptr<Player> GetPlayer() const;
    void Die();
    // 残りの角度のrateの割合だけプレイヤーの方に近づく(本体は左右、銃は上下)
    void TurnToPlayerLerp(float rate);
    // 1フレームに最大maxAngle(ラジアン)だけプレイヤーの方に回る(本体は左右、銃は上下)
    void TurnToPlayerLimited(float maxAngle);
    // プレイヤーが射程内にいて、壁越しでなく直接見えているか
    bool CanSeePlayer() const;
    // 銃口(2本の銃身の真ん中の先端)のワールド座標
    Vector3 GetMuzzlePos() const;
    // 銃が今向いている方向(単位ベクトル)
    Vector3 GetMuzzleDir() const;
    // startからdir方向に伸ばしたレーザーの終点(プレイヤーか壁に当たればそこで止まる)
    Vector3 GetLaserEnd(const Vector3& start, const Vector3& dir) const;
    // startからdir方向に撃ち、プレイヤーに当たっていたらダメージを与える
    void Fire(const Vector3& start, const Vector3& dir);

    void DropSpecialNode();
};
