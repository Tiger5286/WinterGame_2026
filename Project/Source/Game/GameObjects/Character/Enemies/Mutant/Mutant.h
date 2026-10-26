#pragma once
#include "../EnemyBase.h"

class Model;

class Mutant :
    public EnemyBase
{
public:
    Mutant(std::shared_ptr<Player> pPlayer);
    ~Mutant();

    void Init() override;
    void Update() override;
    void Draw() override;

    void OnWasShot(const CollisionManager::ShotInfo& info) override;

private:
    std::unique_ptr<Model> m_pModel;
};

