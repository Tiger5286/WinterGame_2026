#pragma once
#include "Utility/Vector3.h"
#include "Component.h"
class Transform :
    public Component
{
public:
    Vector3 pos = Vector3::Zero();
    Vector3 rot = Vector3::Zero();
    Vector3 scale = Vector3::One();
};