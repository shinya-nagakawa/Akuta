#pragma once
#include <list>
#include "../Base/Base.h"

// �o�H�T�������̊Ǘ��N���X
class NavEdit : public Base
{
private:
	void ControllCamera();
public:

	NavEdit();
	~NavEdit();

	void Update();
	void Render();
	void Draw();
};