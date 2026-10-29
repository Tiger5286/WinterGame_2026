#include "Hackable.h"
#include "Game.h"
#include "DxLib.h"
#include "System/PadInput.h"
#include <algorithm>
#include "Utility/MyLib.h"
#include <cassert>
#include <string>

namespace
{
	// 描画するときの1マスのサイズ
	constexpr int kNodeSize = 70;
	// 最大ハッキング時間
	constexpr int kMaxHackingFrame = 60 * 20;
	
	constexpr int kMinHackFrame = 60 * 3;
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
	m_board.assign(data.size.y, std::vector<NodeType>(data.size.x, NodeType::None));
	m_board[data.goalPos.y][data.goalPos.x] = NodeType::Goal;

	// 開始位置に戻す
	m_pos.resize(1);
	m_pos.back() = data.startPos;

	// 候補をシャッフルして、先頭からnum個のマスを指定の種類にする
	// (空いているマスが足りずに無限ループすることがないように、候補のリストから選ぶ)
	auto placeRandom = [this](std::vector<Vector2Int> candidates, int num, NodeType type)
	{
		for (int i = static_cast<int>(candidates.size()) - 1; i > 0; i--)
		{
			std::swap(candidates[i], candidates[GetRand(i)]);
		}
		for (int i = 0; i < num && i < static_cast<int>(candidates.size()); i++)
		{
			m_board[candidates[i].y][candidates[i].x] = type;
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
			const Vector2Int pos(x, y);
			if (pos == data.startPos || pos == data.goalPos) continue;
			if (std::find(data.redCandidates.begin(), data.redCandidates.end(), pos) != data.redCandidates.end()) continue;
			blueCandidates.push_back(pos);
		}
	}
	placeRandom(blueCandidates, data.blueNum, NodeType::Open);
}

void Hackable::Update()
{
	if (m_hackFrame > 0)
	{
		m_hackFrame--;
	}

	// 赤マスを踏んだら開始位置からやり直し
	const Vector2Int& nowPos = m_pos.back();
	if (m_board[nowPos.y][nowPos.x] == NodeType::Red)
	{
		m_pos.resize(1);
		m_pos.back() = m_boardData.startPos;
		return;
	}

	if (m_pos.back() == m_boardData.goalPos)
	{
		HackedData data = {};
		m_info.func(data);

		int openNum = 0;
		for (const auto& pos : m_pos)
		{
			if (m_board[pos.y][pos.x] == NodeType::Open)
			{
				openNum++;
			}
		}
		if (openNum > 0)
		{
			m_hackFrame += kMaxHackingFrame / m_boardData.blueNum * openNum;
		}
		else
		{
			m_hackFrame += kMinHackFrame;
		}
		m_hackFrame = std::clamp(m_hackFrame, 0, kMaxHackingFrame);
		Init();
	}
}

void Hackable::UpdateHacking()
{
	Move();
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
			if (m_board[y][x] == NodeType::Open)
			{
				color = 0x0000ff;
			}
			else if (m_board[y][x] == NodeType::Red)
			{
				color = 0xff0000;
			}
			// 通った位置は白くする
			for (auto& pos : m_pos)
			{
				if (y == pos.y && x == pos.x)
				{
					color = 0xffffff;
					if (m_board[y][x] == NodeType::Open)
					{
						color = 0x00ffff;
					}
				}
			}
			if (y == m_boardData.goalPos.y && x == m_boardData.goalPos.x)
			{
				color = 0x00ff00;
			}
			DrawBox(x1, y1, x2 - 2, y2 - 2, color, true);
		}
	}
	// ハッキング残り時間の描画
	// ゲージの高さは盤面の高さに合わせる
	const int boardHeight = kNodeSize * m_boardData.size.y;
	float rate = static_cast<float>(m_hackFrame) / static_cast<float>(kMaxHackingFrame);
	int x1, y1, x2, y2;
	x1 = leftTop.x - 50;
	y1 = leftTop.y + boardHeight - 2 - boardHeight * rate;
	x2 = leftTop.x - 2;
	y2 = leftTop.y + boardHeight - 2;
	DrawBox(x1, y1, x2, y2, 0x0088ff, true);
}

bool Hackable::LoadData(const std::wstring& filePath)
{
	const auto table = MyLib::LoadCSV(filePath);
	if (table.empty())
	{
		assert(false && "Hackable::LoadData() : CSVが空か読み込めませんでした");
		return false;
	}

	BoardData data;
	bool hasStart = false;
	bool hasGoal = false;
	int normalNum = 0;	// 青マスを置ける普通のマスの数

	// 盤面の行は設定の行の後に並んでいるので、盤面だけのy座標を別で数える
	int y = 0;
	for (const auto& row : table)
	{
		// 設定の行("blueNum,2"のように1列目が名前、2列目が値)
		if (row[0] == L"blueNum" || row[0] == L"redNum")
		{
			if (row.size() < 2)
			{
				assert(false && "Hackable::LoadData() : 設定の値がありません");
				return false;
			}
			const int value = std::stoi(row[1]);
			if (row[0] == L"blueNum") data.blueNum = value;
			else data.redNum = value;
			continue;
		}

		// 盤面の行。すべての行の長さがそろっているか確認する
		if (y == 0)
		{
			data.size.x = static_cast<int>(row.size());
		}
		else if (static_cast<int>(row.size()) != data.size.x)
		{
			assert(false && "Hackable::LoadData() : 盤面の行の長さがそろっていません");
			return false;
		}

		for (int x = 0; x < static_cast<int>(row.size()); x++)
		{
			const std::wstring& cell = row[x];
			if (cell == L"S")
			{
				if (hasStart)
				{
					assert(false && "Hackable::LoadData() : 開始位置(S)が複数あります");
					return false;
				}
				data.startPos = Vector2Int(x, y);
				hasStart = true;
			}
			else if (cell == L"G")
			{
				if (hasGoal)
				{
					assert(false && "Hackable::LoadData() : ゴール(G)が複数あります");
					return false;
				}
				data.goalPos = Vector2Int(x, y);
				hasGoal = true;
			}
			else if (cell == L"r")
			{
				data.redCandidates.push_back(Vector2Int(x, y));
			}
			else if (cell == L".")
			{
				normalNum++;
			}
			else
			{
				assert(false && "Hackable::LoadData() : 盤面に知らない記号があります");
				return false;
			}
		}
		y++;
	}
	data.size.y = y;

	// 読み込んだ内容のチェック
	if (data.size.x == 0 || data.size.y == 0)
	{
		assert(false && "Hackable::LoadData() : 盤面がありません");
		return false;
	}
	if (!hasStart || !hasGoal)
	{
		assert(false && "Hackable::LoadData() : 開始位置(S)かゴール(G)がありません");
		return false;
	}
	if (data.blueNum < 0 || data.blueNum > normalNum)
	{
		assert(false && "Hackable::LoadData() : 青マスの数が普通のマス(.)の数より多いです");
		return false;
	}
	if (data.redNum < 0 || data.redNum > static_cast<int>(data.redCandidates.size()))
	{
		assert(false && "Hackable::LoadData() : 赤マスの数が候補(r)の数より多いです");
		return false;
	}

	m_boardData = data;
	return true;
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
