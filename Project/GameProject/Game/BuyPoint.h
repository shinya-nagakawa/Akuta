#pragma once
#include "../Base/Base.h"
#include "UIBase.h"

class BuyPoint :public Base
{
public:
	BuyPoint(const CVector3D& pos);
	~BuyPoint();
	int PlayerMoneyMax;
	int count;
	int Speedcount;
	bool buyflag;
	static bool curflag;

	void Update();
	void Render();
	void Draw();
	void Collision(Base* b);

	UIBase* m_ui_frame;
	BuyPoint* m_BuyPoint = nullptr;
	float m_member_float;

private:
};