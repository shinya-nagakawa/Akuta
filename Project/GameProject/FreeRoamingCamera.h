#pragma once
#include "Base/Base.h"

class FreeRoamingCamera :public Base
{
public:
	FreeRoamingCamera();

	void Update();

	// **ˆÊ’u‚Æ‰ñ“]‚ğæ“¾‚·‚éŠÖ”‚ğ’Ç‰Á**
	CVector3D GetPosition() const { return m_pos; }
	CVector3D GetRotation() const { return m_rot; }

	// ˆÊ’u‚Æ‰ñ“]‚ğİ’è‚·‚éŠÖ”‚à’Ç‰Á
	void SetPosition(const CVector3D& pos) { m_pos = pos; }
	void SetRotation(const CVector3D& rot) { m_rot = rot; }

private:
	float m_moveSpeed;
	float m_rotSpeed;

	float Speed;
};