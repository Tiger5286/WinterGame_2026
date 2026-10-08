#pragma once
#include "Component.h"
#include <vector>
#include "Utility/Vector2Int.h"

class Hackable :
    public Component
{
public:
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

private:
    std::vector<std::vector<NodeType>> m_board;
    std::vector<Vector2Int> m_pos;
};

