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

	float m_alpha; //�����x
	float m_duration; //��������
	float m_timer; 
};