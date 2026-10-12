#include "PolygonCollider.h"
#include "Components/Transform.h"
#include "DxLib.h"
#include "Resource/Model.h"

PolygonCollider::PolygonCollider(Transform& transform, Model* pModel) :
	Collider(Type::Polygon, transform),
	m_pModel(pModel)
{
}

void PolygonCollider::Draw(const unsigned int color)
{
	if (!IsEnable() || !m_pModel)
	{
		return;
	}

	m_pModel->Draw();
}