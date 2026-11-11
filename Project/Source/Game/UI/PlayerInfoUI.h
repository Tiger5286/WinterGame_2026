#pragma once
#include "UIBase.h"

class Player;

class PlayerInfoUI :
    public UIBase
{
public:
    PlayerInfoUI(const std::shared_ptr<Player>& pPlayer);
    ~PlayerInfoUI();

    void Init() override;
    void Update() override;
    void Draw() override;

private:
    std::weak_ptr<Player> m_pPlayer;
};

