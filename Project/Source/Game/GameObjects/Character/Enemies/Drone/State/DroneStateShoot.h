#pragma once
#include "DroneState.h"

// 射撃ステート
// 構える → ゆっくり追う → 光ってから撃つ → 待つ をくり返す
class DroneStateShoot :
    public DroneState
{
public:
    DroneStateShoot(Drone& drone);
    ~DroneStateShoot();

    void Enter() override;
    void Update() override;
    void Exit() override;
    void Draw() override;

    ID GetID() const override { return ID::Shoot; }

private:
    enum class Phase
    {
        Ready,      // すぐプレイヤーの方を向く(lerpで素早く回る)
        Track,      // プレイヤーの方にゆっくり向いていく
        Lock,       // 向きを固定して、光ってから撃つ
        Cooldown,   // 撃った後の待ち(レーザーは消す)
    };

private:
    // 構えから始める
    void StartReady();
    // フェーズを切り替える(経過フレームをリセットする)
    void ChangePhase(Phase phase);
    // 追っている間の、今の点滅の間隔
    int GetTrackBlinkInterval() const;

private:
    Phase m_phase = Phase::Ready;
    int m_frame = 0;
    bool m_isLaserOn = true;    // 点滅で今レーザーが点いているか
    int m_blinkTimer = 0;       // 次に点滅が切り替わるまでの残りフレーム
    Vector3 m_lockedDir;    // 固定したレーザーの向き(Lockの間だけ使う)
};
