#include "Vector3.h"
#include <cmath>

Vector3::Vector3():
	x(0.0f),
	y(0.0f),
	z(0.0f)
{
}

Vector3::Vector3(float x, float y, float z) :
	x(x),
	y(y),
	z(z)
{
}

Vector3::~Vector3()
{
}

void Vector3::Draw(const Vector3& start, const Vector3& end)
{
	DrawLine3D(start.ToDxLib(), end.ToDxLib(), 0xffff00);
	Vector3 revVec = end;
	revVec.Normalize();
	revVec *= 20.0f;
	revVec = end - revVec;
	DrawCone3D(end.ToDxLib(), revVec.ToDxLib(), 10.0f, 8, 0xffff00, 0xffff00, true);
}

float Vector3::Angle() const
{
	return atan2(z, -x) + DX_PI_F / 2;;
}

Vector3 Vector3::GetRandVec()
{
	// -1.0f ~ 1.0f のスカラー値を3つ生成
	float rand[3] = { 0.0f };
	for (auto& value : rand)
	{
		// 0~200 - 100 -> -100~100 / 100 -> -1 ~ 1
		value = GetRand(200);
		value -= 100.0f;
		value /= 100.0f;
	}
	Vector3 vec = Vector3(rand[0], rand[1], rand[2]);
	vec.Normalize();
	return vec;
}

VECTOR Vector3::ToDxLib() const
{
	return { x,y,z };
}

Vector3 Vector3::FromDxLib(const VECTOR& vec)
{
	return Vector3(vec.x, vec.y, vec.z);
}

Vector3::operator VECTOR() const
{
	return VGet(x, y, z);
}

Vector3& Vector3::operator=(const VECTOR& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

float Vector3::SquaredLength() const
{
	return x * x + y * y + z * z;
}

float Vector3::Length() const
{
	return sqrtf(SquaredLength());
}

void Vector3::Normalize()
{
	float length = Length();
	if (length > 0.0f)
	{
		x /= length;
		y /= length;
		z /= length;
	}
}

Vector3 Vector3::Normalized() const
{
	Vector3 ans;
	float length = Length();
	if (length > 0.0f)
	{
		ans.x = x / length;
		ans.y = y / length;
		ans.z = z / length;
	}
	return ans;
}

float Vector3::Dot(const Vector3& v) const
{
	return x * v.x + y * v.y + z * v.z;
}

Vector3 Vector3::Cross(const Vector3& v) const
{
	return Vector3(y * v.z - z * v.y,
				   z * v.x - x * v.z,
				   x * v.y - y * v.x);
}

void Vector3::Lerp(const Vector3& v, float t)
{
	x = std::lerp(x, v.x, t);
	y = std::lerp(y, v.y, t);
	z = std::lerp(z, v.z, t);
}

Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator+(const Vector3& v) const
{
	return Vector3(x + v.x,
				   y + v.y,
				   z + v.z);
}

void Vector3::operator+=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}

Vector3 Vector3::operator-(const Vector3& v) const
{
	return Vector3(x - v.x,
				   y - v.y,
				   z - v.z);
}

void Vector3::operator-=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

Vector3 Vector3::operator*(float s) const
{
	return Vector3(x * s,
				   y * s,
				   z * s);
}

void Vector3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
}

Vector3 Vector3::operator/(float s) const
{
	return Vector3(x / s,
				   y / s,
				   z / s);
}

void Vector3::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
}

bool Vector3::operator==(const Vector3& v) const
{
	if (x == v.x && y == v.y && z == v.z) return true;
	return false;
}

bool Vector3::operator!=(const Vector3& v) const
{
	if (x != v.x || y != v.y || z != v.z) return true;
	return false;
}
