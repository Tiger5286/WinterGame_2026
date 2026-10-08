#include "Hackable.h"
#include "Game.h"
#include "DxLib.h"
#include "System/PadInput.h"

namespace
{
	const Vector2Int kBoardSize = { 4,4 };
	const Vector2Int kGoalPos = { 2,2 };

	constexpr int kNodeSize = 70;

	constexpr int kOpenNodeNum = 2;
}

Hackable::Hackable()
{
}

Hackable::~Hackable()
{
}

void Hackable::Init()
{
	m_board.resize(kBoardSize.y);
	for (auto& x : m_board)
	{
		x.resize(kBoardSize.x);
	}
	for (int y = 0; y < m_board.size(); y++)
	{
		for (int x = 0; x < m_board[y].size(); x++)
		{
			m_board[y][x] = NodeType::None;
		}
	}
	m_board[kGoalPos.y][kGoalPos.x] = NodeType::Goal;
	m_pos.resize(1);
	m_pos.back() = { 0,0 };

	for (int i = 0; i < kOpenNodeNum; i++)
	{
		Vector2Int randPos;
		while (randPos == Vector2Int(0, 0))
		{
			randPos.x = GetRand(kBoardSize.x - 1);
			randPos.y = GetRand(kBoardSize.y - 1);
			if (m_board[randPos.y][randPos.x] == NodeType::None)
			{
				m_board[randPos.y][randPos.x] = NodeType::Open;
			}
			else
			{
				randPos = { 0,0 };
			}
		}
	}
}

void Hackable::Update()
{
	Move();

	if (m_pos.back() == kGoalPos)
	{
		Init();
	}
}

void Hackable::Draw()
{
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
			if (y == kGoalPos.y && x == kGoalPos.x)
			{
				color = 0x00ff00;
			}
			DrawBox(x1, y1, x2 - 2, y2 - 2, color, true);
		}
	}
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
