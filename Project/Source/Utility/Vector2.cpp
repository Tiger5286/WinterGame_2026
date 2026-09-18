#include "Vector2.h"
#include <cmath>

Vector2::Vector2():
	x(0.0f),
	y(0.0f)
{
}

Vector2::Vector2(float x, float y) :
	x(x),
	y(y)
{
}

Vector2::~Vector2()
{
}

void Vector2::Draw(const Vector2& start, const Vector2& end)
{
	DrawLine(start.x, start.y, end.x, end.y, 0xffff00);
}

float Vector2::Angle() const
{
	return atan2(y, x);
}

Vector2 Vector2::GetRandVec()
{
	// -1.0f ~ 1.0f のスカラー値を2つ生成
	float rand[2] = { 0.0f };
	for (auto& value : rand)
	{
		// 0~200 - 100 -> -100~100 / 100 -> -1 ~ 1
		value = GetRand(200);
		value -= 100.0f;
		value /= 100.0f;
	}
	Vector2 vec = Vector2(rand[0], rand[1]);
	vec.Normalize();
	return vec;
}

Vector2 Vector2::Rotate(float angle)
{
	float cosTheta = std::cosf(angle);
	float sinTheta = std::sinf(angle);

	Vector2 result;

	result.x = x * cosTheta - y * sinTheta;
	result.y = x * sinTheta - y * cosTheta;

	return result;
}

Vector2 Vector2::Move(const Vector2& vec)
{
	return { x + vec.x,y + vec.y };
}

float Vector2::SquaredLength() const
{
	return x * x + y * y;
}

float Vector2::Length() const
{
	return sqrtf(SquaredLength());
}

void Vector2::Normalize()
{
	float length = Length();
	if (length > 0.0f)
	{
		x /= length;
		y /= length;
	}
}

Vector2 Vector2::Normalized() const
{
	Vector2 ans;
	float length = Length();
	if (length > 0.0f)
	{
		ans.x = x / length;
		ans.y = y / length;
	}
	return ans;
}

float Vector2::Dot(const Vector2& v) const
{
	return x * v.x + y * v.y;
}

float Vector2::Cross(const Vector2& v) const
{
	return x * v.y - y * v.x;
}

void Vector2::Lerp(const Vector2& v, float t)
{
	x = std::lerp(x, v.x, t);
	y = std::lerp(y, v.y, t);
}

Vector2 Vector2::operator-() const
{
	return Vector2(-x, -y);
}

Vector2 Vector2::operator+(const Vector2& v) const
{
	return Vector2(x + v.x,
				   y + v.y);
}

void Vector2::operator+=(const Vector2& v)
{
	x += v.x;
	y += v.y;
}

Vector2 Vector2::operator-(const Vector2& v) const
{
	return Vector2(x - v.x,
				   y - v.y);
}

void Vector2::operator-=(const Vector2& v)
{
	x -= v.x;
	y -= v.y;
}

Vector2 Vector2::operator*(float s) const
{
	return Vector2(x * s,
				   y * s);
}

void Vector2::operator*=(float s)
{
	x *= s;
	y *= s;
}

Vector2 Vector2::operator/(float s) const
{
	return Vector2(x / s,
				   y / s);
}

void Vector2::operator/=(float s)
{
	x /= s;
	y /= s;
}

bool Vector2::operator==(const Vector2& v) const
{
	if (x == v.x && y == v.y) return true;
	return false;
}

bool Vector2::operator!=(const Vector2& v) const
{
	if (x != v.x || y != v.y) return true;
	return false;
}
