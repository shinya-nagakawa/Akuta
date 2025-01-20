#include "GreenFilter.h"

GreenFilter::GreenFilter() :Base(eRedFilter), m_alpha(0.0f), m_duration(0.0f), m_timer(0.0)
{
	m_img = COPY_RESOURCE("GreenFilter", CImage);
	m_img.SetSize(2000, 2000);
}

GreenFilter::~GreenFilter()
{

}

void GreenFilter::Update()
{
	if (m_timer > 0.0f)
	{
		m_timer -= 1.0f;//�^�C�}�[������
		m_alpha = m_timer / m_duration; //�����x���X�V
	}
	else
	{
		m_alpha = 0.0f; //�^�C�}�[��0�ȉ��Ȃ瓧��
	}
}

void GreenFilter::Draw()
{
	if (m_alpha > 0.0f)
	{
		m_img.SetColor(1, 1, 1, m_alpha);
		m_img.Draw();
	}
}

void GreenFilter::GreenShow(float duration)
{
	m_duration = duration;
	m_timer = duration;
	m_alpha = 1.0f; //�s����
}
