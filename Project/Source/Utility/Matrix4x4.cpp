#include "Matrix4x4.h"
#include <cmath>

Matrix4x4::Matrix4x4() : 
	m00(0.0f),m01(0.0f),m02(0.0f),m03(0.0f),
	m10(0.0f),m11(0.0f),m12(0.0f),m13(0.0f),
	m20(0.0f),m21(0.0f),m22(0.0f),m23(0.0f),
	m30(0.0f),m31(0.0f),m32(0.0f),m33(0.0f)
{
}

Matrix4x4::Matrix4x4(float m00, float m01, float m02, float m03,
					 float m10, float m11, float m12, float m13,
					 float m20, float m21, float m22, float m23,
					 float m30, float m31, float m32, float m33) :
	m00(m00),m01(m01),m02(m02),m03(m03),
	m10(m10),m11(m11),m12(m12),m13(m13),
	m20(m20),m21(m21),m22(m22),m23(m23),
	m30(m30),m31(m31),m32(m32),m33(m33)
{
}

Matrix4x4 Matrix4x4::Identity()
{
	return Matrix4x4(1.0f,0.0f,0.0f,0.0f,
					 0.0f,1.0f,0.0f,0.0f,
					 0.0f,0.0f,1.0f,0.0f,
					 0.0f,0.0f,0.0f,1.0f);
}

Matrix4x4 Matrix4x4::Zero()
{
	return Matrix4x4(0.0f, 0.0f, 0.0f, 0.0f,
					 0.0f, 0.0f, 0.0f, 0.0f,
					 0.0f, 0.0f, 0.0f, 0.0f,
					 0.0f, 0.0f, 0.0f, 0.0f);
}

Matrix4x4 Matrix4x4::GetRotX(float angle)
{
	float cos = cosf(angle);
	float sin = sinf(angle);
	return Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
					 0.0f, cos, -sin, 0.0f,
					 0.0f, sin, cos, 0.0f,
					 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 Matrix4x4::GetRotY(float angle)
{
	float cos = cosf(angle);
	float sin = sinf(angle);
	return Matrix4x4(cos, 0.0f, sin, 0.0f,
					 0.0f, 1.0f, 0.0f, 0.0f,
					 -sin, 0.0f, cos, 0.0f,
					 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 Matrix4x4::GetRotZ(float angle)
{
	float cos = cosf(angle);
	float sin = sinf(angle);
	return Matrix4x4(cos, -sin, 0.0f, 0.0f,
					 sin, cos, 0.0f, 0.0f,
					 0.0f, 0.0f, 1.0f, 0.0f,
					 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 Matrix4x4::GetTranslate(Vector3 v)
{
	Matrix4x4 ans = Identity();
	ans.m03 = v.x;
	ans.m13 = v.y;
	ans.m23 = v.z;
	return ans;
}

Matrix4x4 Matrix4x4::GetScale(Vector3 v)
{
	Matrix4x4 ans = Identity();
	ans.m00 = v.x;
	ans.m11 = v.y;
	ans.m22 = v.z;
	return ans;
}

MATRIX Matrix4x4::ToDxLib() const
{
	return {m00,m10,m20,m30,
			m01,m11,m21,m31,
			m02,m12,m22,m32,
			m03,m13,m23,m33 };
}

Matrix4x4 Matrix4x4::FromDxLib(const MATRIX mat)
{
	return Matrix4x4(mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3],
					 mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3],
					 mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3],
					 mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3]);
}

Matrix4x4::operator MATRIX() const
{
	return ToDxLib();
}

Matrix4x4& Matrix4x4::operator=(const MATRIX& m)
{
	m00 = m.m[0][0]; m01 = m.m[1][0]; m02 = m.m[2][0]; m03 = m.m[3][0];
	m10 = m.m[0][1]; m11 = m.m[1][1]; m12 = m.m[2][1]; m13 = m.m[3][1];
	m20 = m.m[0][2]; m21 = m.m[1][2]; m22 = m.m[2][2]; m23 = m.m[3][2];
	m30 = m.m[0][3]; m31 = m.m[1][3]; m32 = m.m[2][3]; m33 = m.m[3][3];
	return *this;
}

Matrix4x4 Matrix4x4::operator*(Matrix4x4 m) const
{
	return Matrix4x4(
		m.m00 * m00 + m.m01 * m10 + m.m02 * m20 + m.m03 * m30,
		m.m00 * m01 + m.m01 * m11 + m.m02 * m21 + m.m03 * m31,
		m.m00 * m02 + m.m01 * m12 + m.m02 * m22 + m.m03 * m32,
		m.m00 * m03 + m.m01 * m13 + m.m02 * m23 + m.m03 * m33,
		m.m10 * m00 + m.m11 * m10 + m.m12 * m20 + m.m13 * m30,
		m.m10 * m01 + m.m11 * m11 + m.m12 * m21 + m.m13 * m31,
		m.m10 * m02 + m.m11 * m12 + m.m12 * m22 + m.m13 * m32,
		m.m10 * m03 + m.m11 * m13 + m.m12 * m23 + m.m13 * m33,
		m.m20 * m00 + m.m21 * m10 + m.m22 * m20 + m.m23 * m30,
		m.m20 * m01 + m.m21 * m11 + m.m22 * m21 + m.m23 * m31,
		m.m20 * m02 + m.m21 * m12 + m.m22 * m22 + m.m23 * m32,
		m.m20 * m03 + m.m21 * m13 + m.m22 * m23 + m.m23 * m33,
		m.m30 * m00 + m.m31 * m10 + m.m32 * m20 + m.m33 * m30,
		m.m30 * m01 + m.m31 * m11 + m.m32 * m21 + m.m33 * m31,
		m.m30 * m02 + m.m31 * m12 + m.m32 * m22 + m.m33 * m32,
		m.m30 * m03 + m.m31 * m13 + m.m32 * m23 + m.m33 * m33
	);
}

void Matrix4x4::operator*=(const Matrix4x4 m)
{
	*this = *this * m;
}

Vector3 Matrix4x4::operator*(const Vector3 v) const
{
	return Vector3(v.x * m00 + v.y * m01 + v.z * m02 + m03,
				   v.x * m10 + v.y * m11 + v.z * m12 + m13,
				   v.x * m20 + v.y * m21 + v.z * m22 + m23);
}

Vector3 operator*(const Vector3& v, const Matrix4x4& m)
{
	return m * v;
}

void operator*=(Vector3& v, const Matrix4x4& m)
{
	v = m * v;
}