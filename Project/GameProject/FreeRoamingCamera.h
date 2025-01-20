#pragma once
#include "Base/Base.h"

class FreeRoamingCamera :public Base
{
public:
	FreeRoamingCamera();

	void Update();

	// **�ʒu�Ɖ�]���擾����֐���ǉ�**
	CVector3D GetPosition() const { return m_pos; }
	CVector3D GetRotation() const { return m_rot; }

	// �ʒu�Ɖ�]��ݒ肷��֐����ǉ�
	void SetPosition(const CVector3D& pos) { m_pos = pos; }
	void SetRotation(const CVector3D& rot) { m_rot = rot; }

private:
	float m_moveSpeed;
	float m_rotSpeed;

	float Speed;
};