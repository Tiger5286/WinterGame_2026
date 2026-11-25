#include "HackingManager.h"
#include "Game/GameObjects/GameObject.h"
#include "Components/Hackable.h"
#include <map>
#include "GameObjects/Character/Player/Player.h"

namespace
{
	// 各特殊ノードごとの効果
	const std::map<Hackable::SpecialNode, std::function<void(const std::shared_ptr<Player>&)>> kSpecialEffects = {
		{ Hackable::SpecialNode::Trigger, [](const std::shared_ptr<Player>& player) { player->OnActiveTrigger(); }},
		{ Hackable::SpecialNode::Critical, [](const std::shared_ptr<Player>& player) { /*何もしない*/ }},
	};
}

HackingManager::HackingManager()
{
}

HackingManager::~HackingManager()
{
}

void HackingManager::Init(const std::shared_ptr<Player>& pPlayer)
{
	m_pPlayer = pPlayer;
}

void HackingManager::Update()
{
	if (!m_pHackingObject.lock()) return;
	auto hackable = m_pHackingObject.lock()->GetComponent<Hackable>();
	if (!hackable || !hackable->IsEnable()) return;
	auto player = m_pPlayer.lock();
	if (!player) return;

	hackable->SetSpecialNodes(player->GetSpecialNodeTypes(), player->GetSpecialNodeVersion());

	auto hackedData = hackable->UpdateHacking();
	if (hackedData)
	{
		// 各特殊効果を発動
		// 一部の効果はすでに発動済。盤面には所持している種類しか出ないので、ここでは必ず消費できる前提にしている。
		for (const auto& special : hackedData->specials)
		{
			bool result = player->UseSpecialNode(special);
			if (!result)
			{
				assert(false && "HackingManager::Update() : 所持していない種類の特殊ノードを発動しようとしています");
				continue;
			}
			kSpecialEffects.at(special)(player);
		}
		// 使い切った種類を次のフレームで描画しないためバージョンを更新する
		hackable->SetSpecialNodes(player->GetSpecialNodeTypes(), player->GetSpecialNodeVersion());
	}
}

void HackingManager::Draw()
{
	if (!m_pHackingObject.lock()) return;
	auto hackable = m_pHackingObject.lock()->GetComponent<Hackable>();
	if (!hackable || !hackable->IsEnable()) return;
	hackable->Draw();
}