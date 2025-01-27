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
	int m_fieldID;
	float SoundVolume;
	int m_dragonCount;
	int m_enemyCount;
private:
	CImage m_img;
	
	bool m_chatWindowOpen;    // チャットウィンドウの状態
};