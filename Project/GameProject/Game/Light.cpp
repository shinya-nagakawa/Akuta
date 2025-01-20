#include "Light.h"
Light::Light(const CVector3D& pos, int no)
	:Base(eLight), m_time(0), m_color_rev(0)
{
	m_pos = pos;
	m_light_no = no;
	m_model = COPY_RESOURCE("Light", CModelObj);
}

Light::~Light()
{
	if(m_light_no>=0)
	CLight::SetType(m_light_no, CLight::eLight_None);
}

void Light::Update()
{
	m_time++;
	m_color_rev = sin(m_time / 40.0f) * 0.2f + 0.8f;
	if (m_light_no >= 0) {
		CLight::SetType(m_light_no, CLight::eLight_Point);
		CLight::SetPos(m_light_no, m_pos);
		CLight::SetColor(m_light_no, CVector3D(0.4f, 0.3f, 0.2f) * m_color_rev, CVector3D(0.7f, 0.5f, 0.4f) * m_color_rev);
		CLight::SetRange(m_light_no, 8.0f);
	}
}

void Light::Render()
{
	if (CShadow::isDoShadow()) return;

	//深度バッファへの書き込みOFF
	glDepthMask(GL_FALSE);
	//加算ブレンドモードに
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	m_model.RenderBuilbord(CCamera::GetCurrent()->GetBuilbordMatrix());

	m_model.GetMaterial(0)->m_emissive = CVector3D(1.0f, 0.4f, 0.2f) * m_color_rev;
	m_model.SetPos(m_pos);
	m_model.Render();


	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);
}
