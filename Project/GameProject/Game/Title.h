#pragma once
#include "../Base/Base.h"

class Title :public Base {
private:
	CImage m_img;
	CVideo* m_video;
	int m_step;
	float m_displayProgress; //i’»—¦‚ğ‰Â‹‰»

public:
	CFont m_font;
	bool Start;
	static int m_select;
	Title();
	~Title();
	void Update() override;
	void Draw() override;
	float GetLoadingProgress();
};