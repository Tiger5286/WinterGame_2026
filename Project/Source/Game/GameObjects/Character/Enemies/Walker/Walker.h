#pragma once
#include "../EnemyBase.h"
class Walker :
    public EnemyBase
{
public:
    Walker(std::shared_ptr<Player> pPlayer);
    ~Walker();

    void Init() override;
    void Update() override;
    void Draw() override;

private:

    friend class WalkerState;
};

