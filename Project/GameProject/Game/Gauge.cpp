#include "Gauge.h"

Gauge::Gauge(GaugeType gauge_type, float scale) :Base(eUI), m_visibility(true)
{
	m_img = COPY_RESOURCE("Gauge", CImage);
	m_par = 0;
	m_gauge_type = gauge_type;
	m_gauge_scale = scale;
}

void Gauge::Draw()
{
	if (!m_visibility)return;
	//z値が負の数の場合は表示しない
	if (m_pos.z < 0)return;
	struct SGaugeData {
		CRect frame_rect;
		CRect bar_rect;
		CVector2D bar_pos;
	}gauge_data[(unsigned int)GaugeType::eMax] = {
		{CRect(84,578,1483,826),CRect(111,857,1452,1044),CVector2D(29,31)},
		{CRect(99,1098,1469,1284),CRect(120,1353,1448,1502),CVector2D(22,19)}
	};
	SGaugeData* d = &gauge_data[(unsigned int)m_gauge_type];
	//枠の表示
	m_img.SetRect(d->frame_rect.m_left, d->frame_rect.m_top, d->frame_rect.m_right, d->frame_rect.m_bottom);
	m_img.SetSize(d->frame_rect.m_width* m_gauge_scale, d->frame_rect.m_height* m_gauge_scale);
	m_img.SetPos(CVector2D(m_pos.x, m_pos.y));
	m_img.Draw();

	
	//ゲージの表示
	m_img.SetRect(d->bar_rect.m_left, d->bar_rect.m_top, d->bar_rect.m_right - d->bar_rect.m_width * (1.0 - m_par), d->bar_rect.m_bottom);
	m_img.SetSize(d->bar_rect.m_width* m_gauge_scale* m_par, d->bar_rect.m_height* m_gauge_scale);
	m_img.SetPos(CVector2D(m_pos.x, m_pos.y) + d->bar_pos * m_gauge_scale);
	m_img.Draw();
}

void Gauge::SetValue(float par)
{
	m_par = par;
}