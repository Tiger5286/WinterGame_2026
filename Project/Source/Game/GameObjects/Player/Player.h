#pragma once
#include "Game/GameObjects/GameObject.h"

class Model;

class Player :
    public GameObject
{
public:
    Player() = default;
    ~Player() override = default;

    void Init() override;
    void Update() override;
    void Draw() const override;

private:
    std::unique_ptr<Model> m_pModel;
};