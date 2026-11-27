#include "HackingManager.h"
#include "Game/GameObjects/GameObject.h"
#include "Components/Hackable.h"
#include <map>
#include "GameObjects/Character/Player/Player.h"
#include "Components/Transform.h"
#include "System/EffectManager.h"

namespace
{
	// 各特殊ノードごとの効果
	const std::map<SpecialNodes::Type, std::function<void(const std::shared_ptr<Player>&)>> kSpecialEffects = {
		{ SpecialNodes::Type::Trigger, [](const std::shared_ptr<Player>& player) { player->OnActiveTrigger(); }},
		{ SpecialNodes::Type::Critical, [](const std::shared_ptr<Player>& player) { /*何もしない*/ }},
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
	auto obj = m_pHackingObject.lock();
	if (!obj) return;
	auto hackable = m_pHackingObject.lock()->GetComponent<Hackable>();
	if (!hackable || !hackable->IsEnable()) return;
	auto player = m_pPlayer.lock();
	if (!player) return;

	hackable->SetSpecialNodes(player->GetSpecialNodeTypes(), player->GetSpecialNodeVersion());

	auto hackedData = hackable->UpdateHacking();
	if (hackedData)
	{
		// エフェクトを出す
		EffectManager::GetInstance().PlayEffect(L"HackEffect", obj->GetTransform().pos);

		// 各特殊効果を発動
		// Critical などの敵側の効果は UpdateHacking() の中で発動済。盤面には所持している種類しか出ないので、ここでは必ず消費できる前提にしている。
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
		// 使い切った種類をこのフレームで描画しないためバージョンを更新する
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