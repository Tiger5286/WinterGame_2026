#pragma once
#include "GameObject.h"
#include "Utility/Vector3.h"
#include <memory>

class Attack;

/// <summary>
/// プレイヤーの足元に円を出し、一定時間後に爆発するミサイルのクラス
/// </summary>
class Missile :
    public GameObject
{
public:
    Missile(const Vector3& pos);
    ~Missile();

    void Init() override;
    void Update() override;
    void Draw() override;

private:
    // 時間をカウントし、爆発時間、消える時間などを管理する
    int m_frameCount = 0;
    
    std::shared_ptr<Attack> m_pAtk = nullptr;
};