#pragma once
#include "Base/Base.h"

class FreeRoamingCamera :public Base
{
public:
	FreeRoamingCamera();

	void Update();

	// **位置と回転を取得する関数を追加**
	CVector3D GetPosition() const { return m_pos; }
	CVector3D GetRotation() const { return m_rot; }

	// 位置と回転を設定する関数も追加
	void SetPosition(const CVector3D& pos) { m_pos = pos; }
	void SetRotation(const CVector3D& rot) { m_rot = rot; }

private:
	float m_moveSpeed;
	float m_rotSpeed;

	float Speed;
};