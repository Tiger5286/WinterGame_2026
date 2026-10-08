#pragma once
#include "Component.h"
#include <vector>

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
};

