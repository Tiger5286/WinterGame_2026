#pragma once
#include "../EnemyBase.h"
#include "Components/Hackable.h"

class Model;
class EnemyHPUI;
class GameObjectManager;

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

	// ミサイルを生成するためにGameObjectManagerのポインタを保持しておく
	void SetGameObjectManager(const std::shared_ptr<GameObjectManager>& gameObjectManager) { m_pGameObjectManager = gameObjectManager; }

private:
    void OnHacked(Hackable::HackedData data);
    void RotToPlayer();
    void SetHitbox();
    Vector3 GetStageCenter() const;
    float GetStageRadius() const;

	// 攻撃ステートをランダムに選ぶ。前回の攻撃と同じものは選ばない
    AttackType SelectAttackType();
	// 攻撃ステートを切り替える
	void ChangeAttackState(AttackType type);

private:
    std::unique_ptr<Model> m_pModel;
    int m_diffuseTexIndex = 0;  // ハッキング時に差し替えるテクスチャの番号
    std::shared_ptr<EnemyHPUI> m_pHPUI = nullptr;

    // 最後の攻撃行動を記憶しておく
    // これを使って同じ攻撃行動が連続しないようにする
    AttackType m_lastAttack = AttackType::None;

	// ミサイルを生成するためにGameObjectManagerのポインタを保持しておく
	std::weak_ptr<GameObjectManager> m_pGameObjectManager;

    friend class MutantState;
};

