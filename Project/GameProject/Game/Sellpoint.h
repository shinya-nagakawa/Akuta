#pragma once
#include "Base/Base.h"
#include "Tutorial/TutorialManager.h"
#include "Tutorial/Renderer.h"
#include "Carry.h"


class Sellpoint :public Carry
{
	CModelObj m_model;
	TutorialManager* tutorialmanager;
private:
	float m_totalEarnings;           // �����p���z
	bool m_isSellFrag;               // ���p�t���O
	Renderer m_renderer;             // Renderer�C���X�^���X
	bool m_showEarnings;             // ���z�ʒm��\�����邩�̃t���O
	float m_displayDuration;         // �ʒm�\���̎�������
	float m_elapsedDisplayTime;      // �ʒm�\���̌o�ߎ���
	float m_displayedEarnings;
	bool soundPlayed;
	bool buyPlayed;
	bool isInside;


public:
	Sellpoint(const CVector3D& pos);
	void Update();
	void Render();
	void Collision(Base*b);
	int GetTotalEarnings() const { return m_totalEarnings; }
	bool IsInsideCapsule(Base* b);
	void TriggerEarnings(int earnings);
	 
};