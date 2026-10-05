#include "MyLib.h"
#include "DxLib.h"
#include <algorithm>
#include <cmath>
#include <limits>

float MyLib::GetAngleDiff(float angle1, float angle2)
{
	float diff = angle1 - angle2 - DX_TWO_PI_F;
	while (diff > DX_PI_F) diff -= DX_TWO_PI_F;
	while (diff < -DX_PI_F) diff += DX_TWO_PI_F;
	return diff;
}

MyLib::RayCapsuleResult MyLib::CheckHitLineCapsule(const Vector3& bottom, const Vector3& top, const float radius, const Vector3& start, const Vector3& end)
{
    // ヒットしない場合の初期値を設定する。
    // 指定された半径が負なら形状として扱えないため、このままヒットなしで返す。
    RayCapsuleResult result;
    result.hitPos = Vector3::Zero();
    if (radius < 0.0f) return result;

    // 線分上の点は start + line * t と表せる。t が0ならstart、1ならend。
    // axisはカプセルの中心線で、bottomからtopへ向かうベクトル。
    const Vector3 line = end - start;
    const Vector3 axis = top - bottom;
    const float axisLengthSq = axis.SquaredLength();

    // 線分の始点からカプセル中心線への最近点を求める。
    // 最近点までの距離が半径以内なら、始点はすでにカプセル内部にある。
    // この場合は「線分がカプセル内から始まった」としてstartを返す。
    // bottomからtopまでのどの位置が中心線上の最近点かを表す係数。
    // 0より小さい場合はbottom、1より大きい場合はtopが最近点になる。
    float axisT = 0.0f;
    if (axisLengthSq > 0.0f)
    {
        // start-bottomを軸に射影して係数を求め、有限な中心線の範囲[0, 1]に制限する。
        axisT = std::clamp((start - bottom).Dot(axis) / axisLengthSq, 0.0f, 1.0f);
    }
    // 求めた係数から中心線上の最近点を復元し、始点との距離を調べる。
    const Vector3 closestAxisPoint = bottom + axis * axisT;
    if ((start - closestAxisPoint).SquaredLength() <= radius * radius)
    {
        result.isHit = true;
        result.hitPos = start;
        return result;
    }

    // 交点候補のうち最も線分の始点に近いものを保存する。
    // 線分のパラメータtは小さいほど始点に近い。
    float nearestT = (std::numeric_limits<float>::max)();
    // 球との交点候補を受け取る処理。
    // 0 <= t <= 1 の候補だけが有限線分上にあり、最小のtを更新する。
    const auto acceptSphereRoot = [&](float t)
    {
        if (t >= 0.0f && t <= 1.0f && t < nearestT) nearestT = t;
    };
    // a*t^2 + b*t + c = 0 を解き、得られた解をacceptRootに渡す。
    // aが0以下なら二次方程式として解けないため候補はない。
    const auto solveQuadratic = [&](float a, float b, float c, const auto& acceptRoot)
    {
        if (a <= 0.0f) return;
        // 判別式が負なら交点はない。浮動小数点誤差で接線の判別式が
        // ごく小さな負値になる場合に備え、スケールに応じた許容誤差を設ける。
        float discriminant = b * b - 4.0f * a * c;
        const float tolerance = 1.0e-6f * (b * b + std::abs(4.0f * a * c) + 1.0f);
        if (discriminant < -tolerance) return;
        // 許容誤差内の負値は接線として扱い、平方根が計算できるよう0に丸める。
        discriminant = (std::max)(discriminant, 0.0f);
        const float root = std::sqrt(discriminant);
        const float denominator = 2.0f * a;
        acceptRoot((-b - root) / denominator);
        acceptRoot((-b + root) / denominator);
    };

    // カプセル中央の円柱側面との交点を求める。
    // 線分と中心線の軸方向成分を取り除くと、軸に垂直な平面での
    // 「半径radiusの円」と「線分」の交差問題になる。
    if (axisLengthSq > 0.0f)
    {
        // 線分の始点とカプセル軸のbottomとの差。
        const Vector3 offset = start - bottom;
        // linePerpとoffsetPerpは、それぞれlineとoffsetから軸方向成分を除いたベクトル。
        const Vector3 linePerp = line - axis * (line.Dot(axis) / axisLengthSq);
        const Vector3 offsetPerp = offset - axis * (offset.Dot(axis) / axisLengthSq);
        // |offsetPerp + t * linePerp|^2 = radius^2 を展開して二次方程式にする。
        const float a = linePerp.SquaredLength();
        const float b = 2.0f * offsetPerp.Dot(linePerp);
        const float c = offsetPerp.SquaredLength() - radius * radius;
        // 円柱を無限に延ばした側面との交点を求める。
        // その後、交点がbottomからtopまでの有限な円柱部分にあるか確認する。
        solveQuadratic(a, b, c, [&](float t)
        {
            if (t < 0.0f || t > 1.0f || t >= nearestT) return;
            // 候補tから線分上の交点を求め、軸方向への射影位置を調べる。
            const Vector3 point = start + line * t;
            const float projection = (point - bottom).Dot(axis) / axisLengthSq;
            if (projection >= 0.0f && projection <= 1.0f) nearestT = t;
        });
    }

    // カプセル両端の半球部分との交点を求める。
    // 球との交点を調べれば端の丸い部分を扱える。bottomとtopが同じ位置なら、
    // 円柱側面はなく、この球判定によって半径radiusの球として判定される。
    // 球との交差では、|start + t*line - center|^2 = radius^2 を解く。
    const float lineLengthSq = line.SquaredLength();
    const auto checkEndSphere = [&](const Vector3& center)
    {
        const Vector3 offset = start - center;
        solveQuadratic(lineLengthSq, 2.0f * offset.Dot(line),
            offset.SquaredLength() - radius * radius, acceptSphereRoot);
    };
    checkEndSphere(bottom);
    checkEndSphere(top);

    // 円柱側面または端の球のいずれかに有効な交点があれば、
    // 最も小さいtを使って線分上の最初のヒット位置を復元する。
    if (nearestT != (std::numeric_limits<float>::max)())
    {
        result.isHit = true;
        result.hitPos = start + line * nearestT;
    }
    return result;
}
