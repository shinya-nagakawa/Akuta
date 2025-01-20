#pragma once
#include "../Base/Base.h"

class GreenFilter :public Base
{
public:
	GreenFilter();
	~GreenFilter();

	void Update();
	void Draw();
	void GreenShow(float duration);
private:
	CImage m_img;

	float m_alpha; //�����x
	float m_duration; //��������
	float m_timer;
};