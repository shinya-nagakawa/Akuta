#pragma once
#include "../Base/Base.h"
class Light : public Base {
	CModelObj m_model;
	int m_light_no;
	int m_time;
	float m_color_rev;
public:
	Light(const CVector3D& pos, int no=-1);
	~Light();
	void Update();
	void Render();
};