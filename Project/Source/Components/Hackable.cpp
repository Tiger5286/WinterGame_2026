#include "Hackable.h"
#include "Game.h"
#include "DxLib.h"
#include "System/PadInput.h"
#include <algorithm>
#include "Utility/MyLib.h"
#include <cassert>
#include <string>
#include "System/Time.h"
#include <map>

namespace
{
	// 描画するときの1マスのサイズ
	constexpr int kNodeSize = 70;
	// 最大ハッキング時間
	constexpr int kMaxHackingFrame = 60 * 20;
	
	constexpr int kMinHackFrame = 60 * 3;

	// 最大ダメージ
	constexpr int kMaxDamage = 100;
	// 最低ダメージ
	constexpr int kMinDamage = 20;

	const std::map<Hackable::SpecialNode, std::wstring> kSpecialNames = {
		{ Hackable::SpecialNode::Trigger, L"Trigger" },
		{ Hackable::SpecialNode::Critical, L"Critical"}
	};
}

Hackable::Hackable()
{
}

Hackable::~Hackable()
{
}

void Hackable::Init()
{
	const BoardData& data = m_boardData;

	// 盤面をすべて何もないマスで作り直す
	m_board.resize(m_boardData.size.y);
	for (auto& line : m_board)
	{
		line.resize(m_boardData.size.x);
		for (auto& cell : line)
		{
			cell = {};
		}
	}
	// ゴール位置を設定
	m_board[data.goalPos.y][data.goalPos.x].type = NodeType::Goal;

	// 開始位置に戻す
	m_pos.resize(1);
	m_pos.back() = data.startPos;

	// ランダムにノードを配置する関数　引数(候補の座標、配置するノードの数、配置するノードの種類)
	std::function placeRandom = [this](std::vector<Vector2Int> candidates, int num, NodeType type)
	{
		// 候補の配列をランダムに入れ替える(Fisher-Yatesシャッフル)
		for (int i = candidates.size() - 1; i > 0; i--)
		{
			int j = GetRand(i);
			std::swap(candidates[i], candidates[j]);
		}
		// 配置する数分、ランダムにした配列の順番にノードを置いていく
		for (int i = 0; i < num; i++)
		{
			m_board[candidates[i].y][candidates[i].x].type = type;
		}
	};

	// 特殊ノードをランダムに配置する関数
	std::function placeRandomSpecial = [this](std::vector<Vector2Int> candidates, std::vector<SpecialNode> special)
		{
			if (candidates.size() < special.size())
			{
				assert(false && "Hackable::Init() : 盤面の空きマスの数以上の数の特殊ノードを配置しようとしています");
				return;
			}
			// 候補の配列をランダムに入れ替える(Fisher-Yatesシャッフル)
			for (int i = candidates.size() - 1; i > 0; i--)
			{
				int j = GetRand(i);
				std::swap(candidates[i], candidates[j]);
			}
			// 特殊ノードを置いていく
			for (int i = 0; i < special.size(); i++)
			{
				m_board[candidates[i].y][candidates[i].x].type = NodeType::Special;
				m_board[candidates[i].y][candidates[i].x].special = special[i];
			}
		};

	// 赤マスは候補(r)の中から選ぶ
	placeRandom(data.redCandidates, data.redNum, NodeType::Red);

	// 青マスは開始位置・ゴール・赤マスの候補以外の普通のマス(.)から選ぶ
	std::vector<Vector2Int> blueCandidates;
	for (int y = 0; y < data.size.y; y++)
	{
		for (int x = 0; x < data.size.x; x++)
		{
			// 調べる位置
			const Vector2Int pos(x, y);
			// スタート、ゴール位置なら飛ばす
			if (pos == data.startPos || pos == data.goalPos) continue;
			// 赤マスが出る可能性のある位置なら飛ばす
			if (std::find(data.redCandidates.begin(), data.redCandidates.end(), pos) != data.redCandidates.end()) continue;
			// 青マスの候補の配列に追加する
			blueCandidates.push_back(pos);
		}
	}
	// 青マスを配置する
	placeRandom(blueCandidates, data.blueNum, NodeType::Open);

	// 特殊ノードを置くマスを選ぶ
	std::vector<Vector2Int> specialCandidates;
	for (int y = 0; y < data.size.y; y++)
	{
		for (int x = 0; x < data.size.x; x++)
		{
			// 調べる位置
			const Vector2Int pos(x, y);
			// スタート位置なら飛ばす
			if (pos == data.startPos) continue;
			if (m_board[y][x].type == NodeType::None)
			{
				specialCandidates.push_back(pos);
			}
		}
	}
	// 配置する
	placeRandomSpecial(specialCandidates, m_specialNodes);
}

void Hackable::Update(float timeScale)
{
	m_hackFrame -= timeScale;
	m_hackFrame = (std::max)(m_hackFrame, 0.0f);
}

std::optional<Hackable::HackedData> Hackable::UpdateHacking()
{
	Move();

	// 赤マスを踏んだら開始位置からやり直し
	const Vector2Int& nowPos = m_pos.back();
	if (m_board[nowPos.y][nowPos.x].type == NodeType::Red)
	{
		m_pos.resize(1);
		m_pos.back() = m_boardData.startPos;
		return std::nullopt;
	}
	// ゴール時の処理
	if (m_pos.back() == m_boardData.goalPos)
	{
		HackedData data = {};

		// いくつ青マスを通ったか求める
		int openNum = 0;
		// 通った特殊ノードを求める
		std::vector<SpecialNode> specials;
		for (const auto& pos : m_pos)
		{
			const auto& nodeData = m_board[pos.y][pos.x];
			if (nodeData.type == NodeType::Open)
			{
				openNum++;
			}
			else if (nodeData.type == NodeType::Special)
			{
				specials.push_back(nodeData.special);
			}
		}
		// ダメージを計算
		int damage = (kMaxDamage - kMinDamage) / m_boardData.blueNum;
		damage *= openNum;
		damage += kMinDamage;
		data.damage = damage;
		// 求めた特殊ノードを入れる
		data.specials = specials;

		// ハッキング発動
		m_info.func(data);
		Time::GetInstance().SetTimeScaleSmooth(0.25f, 1.0f, 0.1f, 30);

		// ハッキングが持続する時間を求める
		if (openNum > 0)
		{
			m_hackFrame += kMaxHackingFrame / m_boardData.blueNum * openNum;
		}
		else
		{
			m_hackFrame += kMinHackFrame;
		}
		m_hackFrame = std::clamp(m_hackFrame, 0.0f, static_cast<float>(kMaxHackingFrame));
		// 盤面を初期状態に戻す
		Init();
		return data;
	}
	return std::nullopt;
}

void Hackable::Draw()
{
	// マスの描画
	const Vector2Int leftTop = { 800,200 };
	for (int y = 0; y < m_board.size(); y++)
	{
		for (int x = 0; x < m_board[y].size(); x++)
		{
			int x1, y1, x2, y2;
			x1 = leftTop.x + x * kNodeSize;
			y1 = leftTop.y + y * kNodeSize;
			x2 = x1 + kNodeSize;
			y2 = y1 + kNodeSize;

			unsigned int color = 0x888888;
			if (m_board[y][x].type == NodeType::Open)
			{
				color = 0x0000ff;
			}
			else if (m_board[y][x].type == NodeType::Red)
			{
				color = 0xff0000;
			}
			else if (m_board[y][x].type == NodeType::Special)
			{
				color = 0xffff00;
			}
			// 通った位置は白くする
			for (auto& pos : m_pos)
			{
				if (y == pos.y && x == pos.x)
				{
					// 通った位置は基本白
					color = 0xffffff;
					// 青マスを通れば水色
					if (m_board[y][x].type == NodeType::Open)
					{
						color = 0x00ffff;
					}			// 特殊マスを通れば黄色
					else if (m_board[y][x].type == NodeType::Special)
					{
						color = 0xffff88;
					}
				}
			}
			if (y == m_boardData.goalPos.y && x == m_boardData.goalPos.x)
			{
				color = 0x00ff00;
			}
			DrawBox(x1, y1, x2 - 2, y2 - 2, color, true);
			if (m_board[y][x].type == NodeType::Special)
			{
				DrawString(x1 + 1, y1 + 1, kSpecialNames.at(m_board[y][x].special).c_str(),0x000000);
			}
		}
	}
	// ハッキング残り時間の描画
	// ゲージの高さは盤面の高さに合わせる
	const int boardHeight = kNodeSize * m_boardData.size.y;
	float rate = m_hackFrame / kMaxHackingFrame;
	int x1, y1, x2, y2;
	x1 = leftTop.x - 50;
	y1 = leftTop.y + boardHeight - 2 - boardHeight * rate;
	x2 = leftTop.x - 2;
	y2 = leftTop.y + boardHeight - 2;
	DrawBox(x1, y1, x2, y2, 0x0088ff, true);
}

bool Hackable::LoadData(const std::wstring& filePath)
{
	auto boardData = MyLib::LoadCSV(filePath);

	// 青マスの数を取得
	if (boardData[0][0] == L"blueNum")
	{
		m_boardData.blueNum = std::stoi(boardData[0][1]);
	}
	else
	{
		assert(false && "1行目がblueNumではありませんでした");
		return false;
	}
	// 赤マスの数を取得
	if (boardData[1][0] == L"redNum")
	{
		m_boardData.redNum = std::stoi(boardData[1][1]);
	}
	else
	{
		assert(false && "2行目がredNumではありませんでした");
		return false;
	}
	// ボードをfor文で見る
	int x = 0; int y = 0;
	for (y = 0; y < boardData.size() - 2; y++)
	{
		for (x = 0; x < boardData[2].size(); x++)
		{
			std::wstring node = boardData[y + 2][x];
			if (node == L".")
			{
				// 何もしない
			}
			else if (node == L"S")
			{
				m_boardData.startPos = { x, y };
			}
			else if (node == L"G")
			{
				m_boardData.goalPos = { x,y };
			}
			else if (node == L"r")
			{
				m_boardData.redCandidates.push_back(Vector2Int(x, y));
			}
			else
			{
				assert(false && "ボードの中に不明な記号があります");
				return false;
			}
		}
	}
	m_boardData.size.x = x;
	m_boardData.size.y = y;

	return true;
}

void Hackable::SetSpecialNodes(const std::vector<SpecialNode>& specialNodes, int version)
{
	// バージョンが同じなら何もしない
	if (m_version == version) return;
	// バージョンが違ったら特殊ノードの一覧を更新して初期化
	m_specialNodes = specialNodes;
	m_version = version;
	Init();
}

void Hackable::Move()
{
	auto& input = PadInput::GetInstance();

	if (input.IsTriggerd(XINPUT_BUTTON_A))
	{
		Vector2Int nextPos = m_pos.back();
		nextPos.y++;

		if (m_pos.size() > 1 && nextPos == m_pos[m_pos.size() - 2])
		{	// 移動先がひとつ前のマスなら戻る
			m_pos.pop_back();
		}
		else if (nextPos.y < m_board.size())
		{
			// 移動先をもう通ったかどうか確認
			bool isPassed = false;
			for (const auto& pos : m_pos)
			{
				if (nextPos == pos)
				{
					isPassed = true;
				}
			}
			// 通ってないなら移動
			if (!isPassed)
			{
				// 移動先がボードの範囲内なら移動
				m_pos.push_back(nextPos);
			}
		}
	}
	if (input.IsTriggerd(XINPUT_BUTTON_B))
	{
		Vector2Int nextPos = m_pos.back();
		nextPos.x++;

		if (m_pos.size() > 1 && nextPos == m_pos[m_pos.size() - 2])
		{	// 移動先がひとつ前のマスなら戻る
			m_pos.pop_back();
		}
		else if (nextPos.x < m_board[m_pos.back().y].size())
		{
			// 移動先をもう通ったかどうか確認
			bool isPassed = false;
			for (const auto& pos : m_pos)
			{
				if (nextPos == pos)
				{
					isPassed = true;
				}
			}
			// 通ってないなら移動
			if (!isPassed)
			{
				// 移動先がボードの範囲内なら移動
				m_pos.push_back(nextPos);
			}
		}
	}
	if (input.IsTriggerd(XINPUT_BUTTON_X))
	{
		Vector2Int nextPos = m_pos.back();
		nextPos.x--;

		if (m_pos.size() > 1 && nextPos == m_pos[m_pos.size() - 2])
		{	// 移動先がひとつ前のマスなら戻る
			m_pos.pop_back();
		}
		else if (nextPos.x >= 0)
		{
			// 移動先をもう通ったかどうか確認
			bool isPassed = false;
			for (const auto& pos : m_pos)
			{
				if (nextPos == pos)
				{
					isPassed = true;
				}
			}
			// 通ってないなら移動
			if (!isPassed)
			{
				// 移動先がボードの範囲内なら移動
				m_pos.push_back(nextPos);
			}
		}
	}
	if (input.IsTriggerd(XINPUT_BUTTON_Y))
	{
		Vector2Int nextPos = m_pos.back();
		nextPos.y--;

		if (m_pos.size() > 1 && nextPos == m_pos[m_pos.size() - 2])
		{	// 移動先がひとつ前のマスなら戻る
			m_pos.pop_back();
		}
		else if (nextPos.y >= 0)
		{
			// 移動先をもう通ったかどうか確認
			bool isPassed = false;
			for (const auto& pos : m_pos)
			{
				if (nextPos == pos)
				{
					isPassed = true;
				}
			}
			// 通ってないなら移動
			if (!isPassed)
			{
				// 移動先がボードの範囲内なら移動
				m_pos.push_back(nextPos);
			}
		}
	}
}
