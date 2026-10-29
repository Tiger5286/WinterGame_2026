#pragma once
#include "Component.h"
#include <vector>
#include "Utility/Vector2Int.h"
#include "Utility/Vector3.h"
#include <functional>
#include <string>

class Hackable :
    public Component
{
public:
    struct HackedData
    {
        int damage = 0;
    };
    struct Info
    {
        std::function<void(HackedData)> func;
        Vector3 hackLocalPos;
    };

    enum class NodeType
    {
        None,   // 何もない
        Open,   // オープンノード(青マス)
        Goal,   // ゴールノード
        Red,    // 赤マス(通ったら最初から)
    };

public:
    Hackable();
    ~Hackable();

    void Init();
    void Update();
    void UpdateHacking();
    void Draw();

    void SetInfo(const Info& info) { m_info = info; }

    bool IsHacked() const { return m_hackFrame > 0; }

    const Vector3& GetHackLocalPos() const { return m_info.hackLocalPos; }

    // 無効にするとハッキング対象から外れる
    void SetEnable(bool isEnable) { m_isEnable = isEnable; }
    bool IsEnable() const { return m_isEnable; }

    // CSVから盤面データを読み込む。読み込めたらtrue
    bool LoadData(const std::wstring& filePath);

private:
    // CSVから読み込んだ盤面の設定(Initのたびにここから盤面を作り直す)
    // LoadDataを呼ばなかった場合は初期値の4x4の盤面になる
    struct BoardData
    {
        Vector2Int size = { 4,4 };              // 盤面のサイズ
        Vector2Int startPos = { 0,0 };          // 開始位置(S)
        Vector2Int goalPos = { 2,2 };           // ゴール位置(G)
        int blueNum = 2;                        // 青マスの数
        int redNum = 0;                         // 赤マスの数
        std::vector<Vector2Int> redCandidates;  // 赤マスが置かれる可能性のある位置(r)
    };

private:
    void Move();

private:
    BoardData m_boardData;
    std::vector<std::vector<NodeType>> m_board;
    std::vector<Vector2Int> m_pos;
    int m_hackFrame = 0;
    Info m_info;
    bool m_isEnable = true;
};

