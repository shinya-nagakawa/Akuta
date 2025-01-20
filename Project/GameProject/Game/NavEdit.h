#pragma once
#include <list>
#include "../Base/Base.h"

// Œo˜H’Tõˆ—‚ÌŠÇ—ƒNƒ‰ƒX
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