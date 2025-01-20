#pragma once
#include "Carry.h"

class Weapon :public Carry
{
private:
	//武器のモデル
	CModelObj sword_model;
	//武器の行列
	CMatrix sword_matrix;
	float m_weight;
	

public:
	Weapon(bool Equippsed,const CVector3D& pos,float weight);
	~Weapon();
	
	void NotifyAttack(); //プレイヤーのパターン認識のための関数

	void Update();
	void Render();
	void Collision(Base*b);
	void Equip();
	void Unequip();
	void Setpos(const CVector3D& pos);
	float GetWeight();
	
};
