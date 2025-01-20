#pragma once
#include "../Base/Base.h"


class Game :public Base {
public:
	Game(int FieldId);
	~Game();
	void Update();
	void Draw();
	bool OpenMenu;
	float sunset;
	float SoundVolume;
private:
	CImage m_img;
	
	bool m_chatWindowOpen;    // チャットウィンドウの状態
};