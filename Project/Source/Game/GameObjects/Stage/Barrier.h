#pragma once
#include "../GameObject.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

class Barrier :
    public GameObject
{
public:
    enum class ActionType
    {
        Enable,
		Disable,
        Destroy
    };

    struct ActionInfo
    {
        std::wstring eventName;
        ActionType actionType;
	};
public:
    Barrier(const std::wstring& modelName, ActionType defaultType);
    ~Barrier();

    void Init() override;
    void Update() override;
    void Draw() override;

    void AddEvent(const ActionInfo& actionInfo);
	void Destroy() { m_isDead = true; }

private:
    const std::wstring m_modelName;
	const ActionType m_defaultActionType;

    std::unique_ptr<Model> m_pModel;

	std::unordered_map<ActionType, std::function<void()>> m_actionMap;

	std::vector<ActionInfo> m_eventActionInfos;
};