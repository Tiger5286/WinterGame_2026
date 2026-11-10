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

    enum class AttackType
    {
        None,
        Normal,
        Missile,
        Charge,

        Num
    };

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

	// 攻撃ステートをランダムに選ぶ。前回の攻撃と同じものは選ばない
    AttackType SelectAttackType();
	// 攻撃ステートを切り替える
	void ChangeAttackState(AttackType type);
#ifdef _DEBUG
    // 数字キーでステートを切り替える
    void DebugChangeState();
    // 現在のステートと切り替えキーの一覧を描画する
    void DebugDraw();
#endif

private:
    std::unique_ptr<Model> m_pModel;
    int m_diffuseTexIndex = 0;  // ハッキング時に差し替えるテクスチャの番号
    std::shared_ptr<EnemyHPUI> m_pHPUI = nullptr;
#ifdef _DEBUG
    int m_debugPrevKey = -1;    // 前のフレームで押されていたキー(押しっぱなしで切り替え直さないため)
#endif
    // 最後の攻撃行動を記憶しておく
	// これを使って同じ攻撃行動が連続しないようにする
    AttackType m_lastAttack = AttackType::None;

    friend class MutantState;
};

