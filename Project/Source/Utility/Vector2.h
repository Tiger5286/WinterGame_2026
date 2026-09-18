#pragma once
#include "Dxlib.h"

/// <summary>
/// 2次元ベクトルを表すクラス
/// </summary>
class Vector2
{
public:
	Vector2();
	Vector2(float x, float y);
	~Vector2();

	// ベクトルの成分
	float x, y;

// ----- ユーティリティ関数 -----
	static inline Vector2 Zero()  { return Vector2(0.0f, 0.0f); }
	static inline Vector2 One()	  { return Vector2(1.0f, 1.0f); }
	static inline Vector2 Up()    { return Vector2(0.0f, 1.0f); }
	static inline Vector2 Down()  { return Vector2(0.0f, -1.0f); }
	static inline Vector2 Right() { return Vector2(1.0f, 0.0f); }
	static inline Vector2 Left()  { return Vector2(-1.0f, 0.0f); }

	static void Draw(const Vector2& start, const Vector2& end);

	/// <summary>
	/// 自身のベクトルのXY成分を見て角度を返す
	/// </summary>
	/// <returns>角度(ラジアン)</returns>
	float Angle() const;

	/// <summary>
	/// ランダムな方向の単位ベクトルを返す
	/// </summary>
	/// <returns>ランダムな方向の単位ベクトル</returns>
	static Vector2 GetRandVec();

	/// <summary>
	/// 回転したベクトルを返す
	/// </summary>
	/// <param name="angle">回転量(ラジアン)</param>
	/// <returns>回転後のベクトル</returns>
	Vector2 Rotate(float angle);

	/// <summary>
	/// 平行移動したベクトルを返す
	/// </summary>
	/// <param name="vec">移動量</param>
	/// <returns>移動後のベクトル</returns>
	Vector2 Move(const Vector2& vec);

// ----- 計算関数 -----
	// ベクトルの大きさの二乗を返す
	float SquaredLength() const;
	// ベクトルの大きさを返す
	float Length() const;

	// 自身のベクトルを正規化する
	void Normalize();
	// 正規化されたベクトルを返す
	Vector2 Normalized() const;

	// ベクトルの内積を返す
	float Dot(const Vector2& v) const;
	// ベクトルの外積を返す
	float Cross(const Vector2& v) const;

	// ベクトルの線形補間をする
	void Lerp(const Vector2& v, float t);

// ----- 演算子オーバーロード -----
	// 反転したベクトルを返す
	Vector2 operator-() const;
	// ベクトルの足し算
	Vector2 operator+(const Vector2& v) const;
	void operator+=(const Vector2& v);
	// ベクトルの引き算
	Vector2 operator-(const Vector2& v) const;
	void operator-=(const Vector2& v);
	// ベクトルの掛け算
	Vector2 operator*(float s) const;
	void operator*=(float s);
	// ベクトルの割り算
	Vector2 operator/(float s) const;
	void operator/=(float s);

	// ベクトルが同じかどうか
	bool operator==(const Vector2& v) const;
	// ベクトルが同じでないかどうか
	bool operator!=(const Vector2& v) const;
};

