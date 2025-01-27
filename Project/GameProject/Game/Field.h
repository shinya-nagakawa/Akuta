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

	int SellCount; // ���p���ꂽ�G�̐�
	bool isFinished; // �I���t���O
public:
	Field(int area);
	~Field();
	void Update();
	void Render();

	void IncrementSellCount();
	int GetSellCount() {
		return SellCount;
	}
	

	CModel* GetModel() 
	{
		return mp_col_model;
	}
	CModel* GetNavModel()const {
		return mp_colNavModel;
	}
};