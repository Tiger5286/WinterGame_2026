#include "Vector2Int.h"

Vector2Int::Vector2Int() :
	x(0),
	y(0)
{
}

Vector2Int::Vector2Int(int x, int y) :
	x(x),
	y(y)
{
}

Vector2Int::~Vector2Int()
{
}

bool Vector2Int::operator==(const Vector2Int& v) const
{
	return x == v.x && y == v.y;
}
