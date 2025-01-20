#include "RedFilter.h"

RedFilter::RedFilter() :Base(eRedFilter),m_alpha(0.0f), m_duration(0.0f), m_timer(0.0)
{
	m_img = COPY_RESOURCE("RedFilter", CImage);
	m_img.SetSize(2000, 2000);
}

RedFilter::~RedFilter()
{
	
}

void RedFilter::Update()
{
	if (m_timer > 0.0f)
	{
		m_timer -= 1.0f;//タイマーを減少
		m_alpha = m_timer / m_duration; //透明度を更新
	}
	else
	{
		m_alpha = 0.0f; //タイマーが0以下なら透明
	}
}

void RedFilter::Draw()
{
	if (m_alpha > 0.0f)
	{
		m_img.SetColor(1, 1, 1, m_alpha);
		m_img.Draw();
	}
}

void RedFilter::Show(float duration)
{
	m_duration = duration;
	m_timer = duration;
	m_alpha = 1.0f; //不透明
}
