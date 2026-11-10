#include "BattleArea.h"
#include "System/EventManager.h"
#include "Game/EnemyPlacer.h"
#include "Game/GameObjects/Character/Enemies/EnemyBase.h"

BattleArea::BattleArea(const Info& info) :
	m_info(info)
{
}

BattleArea::~BattleArea()
{
}

void BattleArea::Init()
{
	// 戦闘開始イベントを受け取ったら、戦闘開始するように登録する。自身が破棄されるときに登録解除されるように、shared_from_this()を渡す
	EventManager::GetInstance().Register(m_info.startEventName, [this]() { StartBattle(); }, shared_from_this());
}

void BattleArea::Update()
{
	// 戦闘開始前は何もしない
	if (!m_isStarted)
	{
		return;
	}

	// 敵が全滅したかどうかを判定する
	bool isAllEnemiesDead = true;
	for (const auto& enemy : m_pEnemies)
	{
		if (auto pEnemy = enemy.lock())
		{
			if (!pEnemy->IsDead())
			{
				isAllEnemiesDead = false;
				break;
			}
		}
	}
	// 敵が全滅したら戦闘終了イベントを発行する
	if (isAllEnemiesDead)
	{
		// 戦闘終了イベントを発行する
		EventManager::GetInstance().CallEvent(m_info.endEventName);
		// 自身を破棄するために死亡フラグを立てる
		m_isDead = true;
	}
}

void BattleArea::Draw()
{
	// 戦闘エリアには見た目がないため何もしない
}

void BattleArea::StartBattle()
{
	// すでに戦闘が開始されている場合は何もしない
	if (m_isStarted)
	{
		return;
	}
	// 敵を配置するために必要なGameObjectManagerとPlayerのポインタを取得する
	auto gameObjectManager = m_info.pGameObjectManager.lock();
	auto player = m_info.pPlayer.lock();
	// GameObjectManagerまたはPlayerが破棄されている場合は、敵を配置できないため、assertでエラーを出す
	if (!gameObjectManager || !player)
	{
		assert(false && "BattleArea::StartBattle() : GameObjectManagerまたはPlayerが破棄されているため、敵を配置できません");
		return;
	}
	// 戦闘開始フラグを立て、敵の監視を開始する
	m_isStarted = true;
	// 敵を配置するための情報を準備
	EnemyFactory::Info info;
	info.pGameObjectManager = gameObjectManager;
	info.pPlayer = player;
	// 敵を配置する
	auto enemies = EnemyPlacer::Place(info, m_info.csvFilePath);
	// 配置した敵を監視対象に追加する
	for (const auto& enemy : enemies)
	{
		m_pEnemies.push_back(enemy);
	}
}
