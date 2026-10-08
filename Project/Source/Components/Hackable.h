#pragma once
#include "Component.h"
#include <vector>
#include "Utility/Vector2Int.h"
#include <functional>

class Hackable :
    public Component
{
public:
    struct HackedData
    {
        int damage = 0;
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
    void Draw();

    void SetFunc(std::function<void(HackedData)> func) { m_goalAction = func; }

private:
    void Move();

private:
    std::vector<std::vector<NodeType>> m_board;
    std::vector<Vector2Int> m_pos;
    std::function<void(HackedData)> m_goalAction;
};

