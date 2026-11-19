#pragma once
#include "../EnemyBase.h"
#include "Components/Hackable.h"

class Model;
class EnemyHPUI;

class Drone :
    public EnemyBase
{
public:
    static constexpr int kMaxHp = 300;
    // 足元(Transformの位置)から本体の中心までの高さ
    static constexpr float kBodyCenterHeight = 35.0f;

public:
    Drone(std::shared_ptr<Player> pPlayer);
    ~Drone();

    void Init() override;
    void Update() override;
    void Draw() override;

    void OnWasShot(const CollisionManager::ShotInfo& info) override;

    // 落下ステート中かどうか
    bool IsFalling();

private:
    void OnHacked(Hackable::HackedData data);
    void SetHitbox();
    // モデルの位置と回転をTransformに合わせる
    void ApplyModelTransform();
    // 銃のフレームを今の上下の角度に合わせて回す
    void ApplyTurretPitch();
    // ハック中の見た目(目の色、チラつき、揺れ)を更新する
    void UpdateHackedLook(bool isHacked);

    // プレイヤーの方を向くための目標の角度
    // 本体の左右の角度(ラジアン)
    float GetTargetYaw();
    // 銃の上下の角度(ラジアン、上が+)。上下の制限をかけた値を返す
    float GetTargetPitch() const;
    // 残りの角度のrateの割合だけプレイヤーの方に近づく(本体は左右、銃は上下)
    void TurnToPlayerLerp(float rate);
    // 1フレームに最大maxAngle(ラジアン)だけプレイヤーの方に回る(本体は左右、銃は上下)
    void TurnToPlayerLimited(float maxAngle);

    // プレイヤーが射程内にいて、壁越しでなく直接見えているか
    bool CanSeePlayer() const;
    // 銃口(2本の銃身の真ん中の先端)のワールド座標
    Vector3 GetMuzzlePos() const;
    // 銃が今向いている方向(ワールド、単位ベクトル)
    Vector3 GetMuzzleDir() const;
    // プレイヤーの体の真ん中のワールド座標
    Vector3 GetPlayerCenterPos() const;
    // startからdir方向に伸ばしたレーザーの終点
    // プレイヤーか壁に当たればそこで止まる。isHitPlayerにはプレイヤーに当たったかが入る
    Vector3 GetLaserEnd(const Vector3& start, const Vector3& dir, bool* isHitPlayer = nullptr) const;
    // startからdir方向に撃ち、プレイヤーに当たっていたらダメージを与える
    void Fire(const Vector3& start, const Vector3& dir);

private:
    std::unique_ptr<Model> m_pModel;
    int m_turretFrameIndex = -1;
    MATRIX m_turretDefaultMatrix = {};               // 銃のフレームの元のローカル行列
    float m_turretPitch = 0.0f;                 // 銃の上下の角度(ラジアン、上が+)
    // 目のマテリアル(ハック中に色を変える)
    int m_eyeMaterialIndex = -1;
    COLOR_F m_eyeDefaultDifColor = {};               // 目の元の色
    COLOR_F m_eyeDefaultEmiColor = {};               // 目の元の発光色
    // ハック中の故障っぽい演出
    float m_glitchWaitFrame = 0.0f;                  // 次にチラつくまでの残りフレーム
    float m_glitchFrame = 0.0f;                      // チラつきの残りフレーム(0ならチラついていない)
    Vector3 m_shakeOffset;                      // 描画位置だけをずらす揺れ
    std::shared_ptr<EnemyHPUI> m_pHPUI = nullptr;

    friend class DroneState;
};
