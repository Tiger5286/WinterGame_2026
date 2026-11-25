#pragma once
#include "GameObject.h"
#include "Utility/Vector3.h"
#include "Components/Hackable.h"
#include <memory>

class Player;

// プレイヤーが取得できる特殊ノードを表すクラス
class SpecialNodeItem :
    public GameObject
{
public:
    SpecialNodeItem(const Vector3& pos, SpecialNodes::Type type, const std::shared_ptr<Player>& pPlayer);
    ~SpecialNodeItem();

    void Init() override;
    void Update() override;
    void Draw() override;

private:
    bool TryPickup();

private:
    SpecialNodes::Type m_type;
    std::weak_ptr<Player> m_pPlayer;
    int m_count;
#ifdef _DEBUG
    bool m_isCanPickupDist = false;
#endif
};

