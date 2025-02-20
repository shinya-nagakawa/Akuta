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

	float m_alpha; //透明度
	float m_duration; //持続時間
	float m_timer;
};