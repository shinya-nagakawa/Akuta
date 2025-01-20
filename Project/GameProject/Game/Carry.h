#pragma once
#include "../Base/Base.h"

class Carry : public Base
{
protected:
	
public:
	
	int m_stateItem;
	enum
	{
		e_Drop,
		e_Equip,
		e_PickUp,
	};
	//bool m_isPickuped;

	Carry(int type);
	void Equip() { m_stateItem = e_Equip; };
	void PutInPocket() { m_stateItem = e_PickUp; };
	void Unequip() { m_stateItem = e_Drop; };
};