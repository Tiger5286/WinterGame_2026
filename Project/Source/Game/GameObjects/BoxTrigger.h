#pragma once
#include "GameObject.h"
#include <string>

class Player;

class BoxTrigger :
    public GameObject
{
public:
    BoxTrigger(const Vector3& pos, const Vector3& size,const std::shared_ptr<Player>& pPlayer, const std::wstring& eventName);
    ~BoxTrigger();

    void Init() override;
    void Update() override;
    void Draw() override;

private:
    // ボックスのサイズ
	// Transform.posからの相対座標で指定する
    Vector3 m_size;
    std::weak_ptr<Player> m_pPlayer;

    bool m_isPlayerEntered = false;

	std::wstring m_enterEventName;
};

