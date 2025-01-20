#pragma once
#include "../Base/Base.h"

class Gauge :public Base
{
public:
	enum class GaugeType {
		ePlayerGauge,
		ePlayerStamina,
		eMax
	};
private:
	float m_par;
	GaugeType m_gauge_type;
	CImage m_img;
	float m_gauge_scale;
	bool m_visibility;
public:
	Gauge(GaugeType gauge_type, float scale);
	void Draw();
	void SetValue(float par);
	void SetVisibility(bool b)
	{
		m_visibility = b;
	}
};
