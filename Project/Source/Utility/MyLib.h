#pragma once
#include "Vector3.h"
#include <vector>
#include <string>

namespace MyLib
{
	/// <summary>
	/// 二つの角度の最短の差を求める
	/// </summary>
	/// <param name="angle1">角度(ラジアン)</param>
	/// <param name="angle2">角度(ラジアン)</param>
	/// <returns>二つの角度の最短の差</returns>
	float GetAngleDiff(float angle1, float angle2);

	/// <summary>
	/// レイとカプセルの当たり判定の結果が入る構造体
	/// </summary>
	struct RayCapsuleResult
	{
		bool isHit = false;
		Vector3 hitPos;
		// 線分の最近点までの距離
		float dist = 0.0f;
	};

	/// <summary>
	/// レイとカプセルの当たり判定をする
	/// </summary>
	/// <param name="bottom">カプセルの線分の端点1</param>
	/// <param name="top">カプセルの線分の端点2</param>
	/// <param name="radius">カプセルの半径</param>
	/// <param name="start">レイの端点1</param>
	/// <param name="end">レイの端点2</param>
	/// <returns>当たり判定の結果</returns>
	RayCapsuleResult CheckHitLineCapsule(const Vector3& bottom, const Vector3& top, const float radius, const Vector3& start, const Vector3& end);
	/// <summary>
	/// レイと球の当たり判定をする
	/// </summary>
	/// <param name="pos">球の位置</param>
	/// <param name="radius">球の半径</param>
	/// <param name="start">レイの端点1</param>
	/// <param name="end">レイの端点2</param>
	/// <returns>当たり判定の結果</returns>
	RayCapsuleResult CheckHitLineSphere(const Vector3& pos, const float radius, const Vector3& start, const Vector3& end);

	/// <summary>
	/// csvを読み込み、配列に格納して返す。頭に#がある行は読み飛ばされる。
	/// </summary>
	/// <param name="filePath">読み込むcsvファイルのパス</param>
	/// <returns>情報が格納された配列</returns>
	std::vector<std::vector<std::wstring>> LoadCSV(const std::wstring& filePath);
}