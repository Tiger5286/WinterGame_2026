#pragma once
class Vector2Int
{
public:
	Vector2Int();
	Vector2Int(int x, int y);
	~Vector2Int();

	int x, y;

	bool operator==(const Vector2Int& v) const;
};

