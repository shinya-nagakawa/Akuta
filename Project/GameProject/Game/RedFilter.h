#pragma once
#include "../Base/Base.h"

class RedFilter:public Base
{
public:
	RedFilter();
	~RedFilter();

	void Update();
	void Draw();
	void Show(float duration);
private:
	CImage m_img;

	float m_alpha; //透明度
	float m_duration; //持続時間
	float m_timer; 
};