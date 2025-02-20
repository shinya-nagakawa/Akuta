#pragma once
#include "../Base/Base.h"

class BlackFilter :public Base
{
public:
	BlackFilter(float time =0);
	~BlackFilter();

	void Update();
	void Draw();
	void BlackShow(float duration);

	bool CheckEnd() { return m_timer <= 0; }
private:
	CImage m_img;

	float m_alpha; //透明度
	float m_duration; //持続時間
	float m_timer;
};