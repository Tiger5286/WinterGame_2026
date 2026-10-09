#pragma once
#include "Component.h"
#include <vector>
#include "Utility/Vector2Int.h"
#include "Utility/Vector3.h"
#include <functional>

class Hackable :
    public Component
{
public:
    struct HackedData
    {
        int damage = 0;
    };
    struct Info
    {
        std::function<void(HackedData)> func;
        Vector3 hackLocalPos;
    };

    enum class NodeType
    {
        None,
        Open,
        Goal
    };

public:
    Hackable();
    ~Hackable();

    void Init();
    void Update();
    void UpdateHacking();
    void Draw();

    void SetInfo(const Info& info) { m_info = info; }

    bool IsHacked() const { return m_hackFrame > 0; }

    const Vector3& GetHackLocalPos() const { return m_info.hackLocalPos; }

private:
    void Move();

private:
    std::vector<std::vector<NodeType>> m_board;
    std::vector<Vector2Int> m_pos;
    int m_hackFrame = 0;
    Info m_info;
};

