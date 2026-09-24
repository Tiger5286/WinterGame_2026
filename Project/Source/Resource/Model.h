#pragma once
#include "Resource.h"
#include "DxLib.h"
#include "Components/Transform.h"
#include "Utility/Matrix4x4.h"

class Model : public Resource
{
public:
	Model(int handle) : Resource(handle)
	{
	}

	~Model()
	{
		MV1DeleteModel(m_handle.Get());
	}

	void Draw() const
	{
		MV1DrawModel(m_handle.Get());
	}

	void SetTransform(const Transform& transform)
	{
		Matrix4x4 scale = Matrix4x4::GetScale(transform.scale);
		Matrix4x4 rotX = Matrix4x4::GetRotX(transform.rot.x);
		Matrix4x4 rotY = Matrix4x4::GetRotY(transform.rot.y);
		Matrix4x4 rotZ = Matrix4x4::GetRotZ(transform.rot.z);
		Matrix4x4 trans = Matrix4x4::GetTranslate(transform.pos);
		Matrix4x4 mat = scale * rotX * rotY * rotZ * trans;
		MV1SetMatrix(m_handle.Get(), mat);
	}
};