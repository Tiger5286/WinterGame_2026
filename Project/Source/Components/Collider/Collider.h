#pragma once
#include "../Component.h"

class Transform;

/// <summary>
/// 当たり判定の情報を持つクラスの基底クラス
/// </summary>
class Collider : public Component
{
public:
	// 当たり判定の種類
	enum class Type
	{
		None,
		Sphere,		// 球
		Capsule,	// カプセル
		Polygon,	// ポリゴン(モデル)

		Num
	};

public:
	Collider(Type type, Transform& transform);
	virtual ~Collider() = default;

	// 当たり判定を描画する
	virtual void Draw() abstract;

	// Transformを取得する
	const Transform& GetTransform() const { return m_transform; }

	// 有効か無効かを設定/取得する
	bool IsEnable() const { return m_isEnable; }
	void SetEnable(bool isEnable) { m_isEnable = isEnable; }

	// 当たり判定の種類を取得する
	Type GetType() const { return m_type; }

protected:
	Transform& m_transform;
	bool m_isEnable = true;

	const Type m_type = Type::None;
};