#pragma once
#include "Collider.h"

class Model;

/// <summary>
/// モデル形状を使うポリゴン当たり判定
/// </summary>
class PolygonCollider final : public Collider
{
public:
	// modelHandleは所有せず、モデル管理側が解放する
	PolygonCollider(Transform& transform, Model* model);

	Model* GetModel() const { return m_pModel; }
	void SetModel(Model* model) { m_pModel = model; }

	void Draw() override;

private:
	Model* m_pModel = nullptr;
};