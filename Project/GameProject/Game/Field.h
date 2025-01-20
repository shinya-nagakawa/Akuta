#pragma once
#include "../Base/Base.h"
#include "Tutorial/TutorialManager.h"
#include "Sellpoint.h"

class Field : public Base{
private:

	TutorialManager* tutorialManager; // �`���[�g���A���Ǘ��p
	Sellpoint* m_sellpoint;

	CModel* mp_model;
	CModel* mp_col_model;
	CModel* mp_colNavModel;	//�i�r�Q�[�V�����p�̃R���W�������f��
public:
	Field(int area);
	~Field();
	void Update();
	void Render();

	CModel* GetModel() 
	{
		return mp_col_model;
	}
	CModel* GetNavModel()const {
		return mp_colNavModel;
	}
};