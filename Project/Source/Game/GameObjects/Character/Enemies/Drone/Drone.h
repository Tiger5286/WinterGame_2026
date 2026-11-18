#pragma once
#include "../EnemyBase.h"
class Drone :
    public EnemyBase
{
public:
    Drone(std::shared_ptr<Player> pPlayer);
    ~Drone();

    void Init() override;
    void Update() override;
    void Draw() override;

private:

};