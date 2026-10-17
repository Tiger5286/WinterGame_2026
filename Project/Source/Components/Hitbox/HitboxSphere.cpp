#include "HitboxSphere.h"
#include "Resource/Model.h"

HitboxSphere::HitboxSphere()
{
}

HitboxSphere::~HitboxSphere()
{
}

void HitboxSphere::Update()
{
	int frameIndex = MV1SearchFrame(m_info.pModel->GetHandle(), m_info.frameName.c_str());
	MATRIX frameWorldMat = MV1GetFrameLocalWorldMatrix(m_info.pModel->GetHandle(), frameIndex);

	MATRIX offsetMat = MGetTranslate(m_info.localOffset);

	MATRIX resultMat = MMult(offsetMat,frameWorldMat);

	m_pos = Vector3::FromDxLib(VTransform(Vector3::Zero(), resultMat));
}

void HitboxSphere::Draw()
{
#ifdef _DEBUG
	unsigned int color = kColor;
	if (m_info.isWeakPoint) color = kWeakPointColor;

	DrawSphere3D(m_pos, m_info.radius, 16, color, 0xffffff, false);
#endif
}

void HitboxSphere::SetInfo(const Info& info)
{
	m_info = info;
}