#include "Laser.h"

void Laser::Draw(const Vector3& start, const Vector3& end, float thickness, int handle, const COLOR_U8 color)
{
	// 計算に必要な値を出す
	const Vector3 toEnd = end - start;	// 線の始点から終点のベクトル
	const Vector3 dir = toEnd.Normalized();	// 線の方向
	const Vector3 centerPos = start + toEnd * 0.5f;	// 線の中点の位置
	const Vector3 toCamera = Vector3::FromDxLib(GetCameraPosition()) - centerPos;	// 線からカメラへのベクトル
	// 板ポリゴンの幅のベクトルを計算
	const Vector3 side = dir.Cross(toCamera).Normalized() * thickness * 0.5f;
	// 板ポリゴンの四隅の位置を計算
	const Vector3 lt = start + side;
	const Vector3 lb = start - side;
	const Vector3 rt = end + side;
	const Vector3 rb = end - side;
	// ポリゴンを作成
	VERTEX3D vtx[6];
	vtx[0].pos = lt; vtx[0].u = 0.0f; vtx[0].v = 0.0f;
	vtx[1].pos = rt; vtx[1].u = 1.0f; vtx[1].v = 0.0f;
	vtx[2].pos = lb; vtx[2].u = 0.0f; vtx[2].v = 1.0f;
	vtx[3].pos = rt; vtx[3].u = 1.0f; vtx[3].v = 0.0f;
	vtx[4].pos = rb; vtx[4].u = 1.0f; vtx[4].v = 1.0f;
	vtx[5].pos = lb; vtx[5].u = 0.0f; vtx[5].v = 1.0f;
	// ポリゴン共通の情報を設定
	for (auto& v : vtx)
	{
		v.dif = color;
		v.norm = Vector3::Front();
		v.spc = GetColorU8(0, 0, 0, 0);
		v.sv = v.su = 0.0f;
	}
	// ポリゴンを描画
	SetUseLighting(false);
	SetWriteZBuffer3D(false);
	SetMaterialUseVertDifColor(true);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	DrawPolygon3D(vtx, 2, handle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetWriteZBuffer3D(true);
	SetUseLighting(true);
}