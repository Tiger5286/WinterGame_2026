#pragma once
#include "GameObject.h"
#include <string>
#include <memory>
#include <vector>

class EnemyBase;
class GameObjectManager;
class Player;

/// <summary>
/// 戦闘エリアを表すクラス。
/// このクラス自体はプレイヤーが戦闘エリアに入ったかどうか判定しない。
/// BoxTriggerが戦闘エリアに入ったかどうかを判定し、戦闘開始イベントを発行する。
/// 戦闘開始イベントを受け取ったら、戦闘エリア内の敵を生成する。
/// 戦闘エリア内の敵が全滅したら、戦闘終了イベントを発行し、自身は破棄される。
/// </summary>
class BattleArea :
    public GameObject
{
public:
    /// <summary>
	/// このクラスを生成するために必要な情報を持った構造体
    /// </summary>
    struct Info
    {
		std::wstring startEventName;    // 戦闘開始イベント名。EventManagerからこのイベントを受け取ったら戦闘開始する
		std::wstring endEventName;      // 戦闘終了イベント名。戦闘終了時にEventManagerにこのイベントを発行する
		std::wstring csvFilePath;       // 敵の配置情報が書かれたcsvファイルのパス
		std::weak_ptr<GameObjectManager> pGameObjectManager;    // GameObjectManagerのポインタ。敵を生成するために必要
		std::weak_ptr<Player> pPlayer;  // プレイヤーのポインタ。敵を生成するために必要
    };

public:
    BattleArea(const Info& info);
    ~BattleArea();

    void Init() override;
    void Update() override;
	void Draw() override;

private:
	// 戦闘開始イベントを受け取ったときに呼ばれる関数
    void StartBattle();

private:
    const Info m_info;
	// 戦闘エリア内の敵の配列。敵が全滅したかどうかを判定するために使用する
    std::vector<std::weak_ptr<EnemyBase>> m_pEnemies;
	bool m_isStarted = false;
};