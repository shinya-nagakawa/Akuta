#pragma once
#include "Carry.h"

class Weapon :public Carry
{
private:
	//����̃��f��
	CModelObj sword_model;
	//����̍s��
	CMatrix sword_matrix;
	float m_weight;
	

public:
	Weapon(bool Equippsed,const CVector3D& pos,float weight);
	~Weapon();
	
	void NotifyAttack(); //�v���C���[�̃p�^�[���F���̂��߂̊֐�

	void Update();
	void Render();
	void Collision(Base*b);
	void Equip();
	void Unequip();
	void Setpos(const CVector3D& pos);
	float GetWeight();
	
};
