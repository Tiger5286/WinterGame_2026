#pragma once
#include <memory>

class Player;
class GameObject;

class HackingManager
{
public:
	HackingManager();
	~HackingManager();

	void Init(const std::shared_ptr<Player>& pPlayer);
	void Update();
	void Draw();

	void SetTarget(std::shared_ptr<GameObject> pTarget) { m_pHackingObject = pTarget; }

private:
	std::weak_ptr<Player> m_pPlayer;
	std::weak_ptr<GameObject> m_pHackingObject;
};