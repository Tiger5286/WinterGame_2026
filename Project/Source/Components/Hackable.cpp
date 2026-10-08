#include "Hackable.h"
#include "Game.h"
#include "DxLib.h"

namespace
{
	const Vector2Int kBoardSize = { 4,4 };
	const Vector2Int kGoalPos = { 2,2 };

	constexpr int kNodeSize = 30;
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
	m_board[kGoalPos.y][kGoalPos.x] = NodeType::Goal;
}

void Hackable::Update()
{
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
			DrawBox(x1, y1, x2, y2, 0xffffff, true);
		}
	}
}