#pragma once
#include "Carry.h"

class Item : public Carry
{
private:
	//•Ší‚Ìƒ‚ƒfƒ‹
	//CModelObj item_model;
	//•Ší‚Ìs—ñ
	CMatrix item_matrix;
	CModelA3M m_model;

	float count;
	float m_weight;

public:
	Item(bool Equippsed, const CVector3D& pos,float weight);
	~Item();


	void Update();
	void Render();
	void Collision(Base* b);
	void Equip();
	void Unequip();
	void Setpos(const CVector3D& pos);

};
