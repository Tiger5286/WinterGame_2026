#pragma once
#include "Dxlib.h"
#include "Vector3.h"

class Matrix4x4
{
public:
	Matrix4x4();
	Matrix4x4(float m00, float m01, float m02, float m03,
			  float m10, float m11, float m12, float m13,
			  float m20, float m21, float m22, float m23,
			  float m30, float m31, float m32, float m33);

	// 単位行列を生成する関数。
	static Matrix4x4 Identity();
	// すべての要素が0の行列を生成する関数。
	static Matrix4x4 Zero();

	// X軸回転行列を生成する。angleは弧度法で指定する。
	static Matrix4x4 GetRotX(float angle);
	// Y軸回転行列を生成する。angleは弧度法で指定する。
	static Matrix4x4 GetRotY(float angle);
	// Z軸回転行列を生成する。angleは弧度法で指定する。
	static Matrix4x4 GetRotZ(float angle);

	// 平行移動行列を生成する関数。
	static Matrix4x4 GetTranslate(Vector3 v);

	// 拡大縮小行列を生成する関数。
	static Matrix4x4 GetScale(Vector3 v);

	// DxLibの行列形式に変換する関数。DxLibの行列は列行になっているため、行列の要素を入れ替える必要がある。
	MATRIX ToDxLib() const;
	// DxLibの行列をこのクラスの行列に変換して返す
	static Matrix4x4 FromDxLib(const MATRIX mat);
	// DxLibの行列に自動変換
	operator MATRIX() const;
	// DxLibの行列を代入できるようにする
	Matrix4x4& operator=(const MATRIX& m);

	// 行列の要素を表すメンバ変数。
	float m00, m01, m02, m03,
		  m10, m11, m12, m13,
		  m20, m21, m22, m23,
		  m30, m31, m32, m33;

	// 行列の掛け算
	Matrix4x4 operator*(const Matrix4x4 m) const;
	void operator*=(const Matrix4x4 m);

	// 行列とベクトルの掛け算
	// 行列 * ベクトル
	Vector3 operator*(const Vector3 v) const;
	// ベクトル * 行列 (結果は行列*ベクトルと同じ)
	friend Vector3 operator*(const Vector3& v, const Matrix4x4& m);
	friend void operator*=(Vector3& v, const Matrix4x4& m);
};

