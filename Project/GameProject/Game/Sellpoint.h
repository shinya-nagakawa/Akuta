#pragma once
#include "Base/Base.h"
#include "Tutorial/TutorialManager.h"
#include "Tutorial/Renderer.h"
#include "Carry.h"


class Sellpoint :public Carry
{
	CModelObj m_model;
	TutorialManager* tutorialmanager;
private:
	float m_totalEarnings;           // 総売却金額
	bool m_isSellFrag;               // 売却フラグ
	Renderer m_renderer;             // Rendererインスタンス
	bool m_showEarnings;             // 総額通知を表示するかのフラグ
	float m_displayDuration;         // 通知表示の持続時間
	float m_elapsedDisplayTime;      // 通知表示の経過時間
	float m_displayedEarnings;
	bool soundPlayed;
	bool buyPlayed;
	bool isInside;


public:
	Sellpoint(const CVector3D& pos);
	void Update();
	void Render();
	void Collision(Base*b);
	int GetTotalEarnings() const { return m_totalEarnings; }
	bool IsInsideCapsule(Base* b);
	void TriggerEarnings(int earnings);
	 
};