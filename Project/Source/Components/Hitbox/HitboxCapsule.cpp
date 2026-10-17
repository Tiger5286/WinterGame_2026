#include "HitboxCapsule.h"
#include "Resource/Model.h"

HitboxCapsule::HitboxCapsule()
{
}

HitboxCapsule::~HitboxCapsule()
{
}

void HitboxCapsule::Update()
{
	const int handle = m_info.pModel->GetHandle();
	int frameIndex = MV1SearchFrame(handle, m_info.bottomFrameName.c_str());
	MATRIX frameWorldMat = MV1GetFrameLocalWorldMatrix(handle, frameIndex);
	MATRIX offsetMat = MGetTranslate(m_info.bottomLocalOffset);
	MATRIX resultMat = MMult(offsetMat, frameWorldMat);
	m_bottomPos = Vector3::FromDxLib(VTransform(Vector3::Zero(), resultMat));

	frameIndex = MV1SearchFrame(handle, m_info.topFrameName.c_str());
	frameWorldMat = MV1GetFrameLocalWorldMatrix(handle, frameIndex);
	offsetMat = MGetTranslate(m_info.topLocalOffset);
	resultMat = MMult(offsetMat, frameWorldMat);
	m_topPos = Vector3::FromDxLib(VTransform(Vector3::Zero(), resultMat));
}

void HitboxCapsule::Draw()
{
#ifdef _DEBUG
	unsigned int color = kColor;
	if (m_info.isWeakPoint) color = kWeakPointColor;

	DrawCapsule3D(m_bottomPos, m_topPos, m_info.radius, 16, color, 0xffffff, false);
#endif
}
