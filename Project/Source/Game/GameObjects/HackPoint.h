#pragma once
#include "GameObject.h"
#include <string>
class HackPoint :
    public GameObject
{
public:
    HackPoint();
    ~HackPoint();

    void Init() override;
    void Update() override;
    void Draw() override;

    void SetEvent(const std::wstring& event);

private:
    void OnHacked();

private:
    std::wstring m_eventName;
};

