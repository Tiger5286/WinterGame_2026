#pragma once
#include "GameObject.h"
#include <string>

class Player;

class BoxTrigger :
    public GameObject
{
public:
    BoxTrigger(const Vector3& pos, const Vector3& pos2,const std::shared_ptr<Player>& pPlayer, const std::wstring& eventName);
    ~BoxTrigger();

    void Init() override;
    void Update() override;
    void Draw() override;

private:
	// ボックスの角の座標を表す2つのベクトル(posとpos2)でボックスを定義する
    Vector3 m_minPos;
    Vector3 m_maxPos;
    Vector3 m_pos2;
    std::weak_ptr<Player> m_pPlayer;

    bool m_isPlayerEntered = false;

	std::wstring m_enterEventName;
};