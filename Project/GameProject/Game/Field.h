#pragma once
#include "../Base/Base.h"
#include "Tutorial/TutorialManager.h"
#include "Sellpoint.h"

class Field : public Base{
private:

	TutorialManager* tutorialManager; // チュートリアル管理用
	Sellpoint* m_sellpoint;

	CModel* mp_model;
	CModel* mp_col_model;
	CModel* mp_colNavModel;	//ナビゲーション用のコリジョンモデル

	int SellCount; // 売却された敵の数
	bool isFinished; // 終了フラグ
public:
	Field(int area);
	~Field();
	void Update();
	void Render();

	void IncrementSellCount();
	int GetSellCount() {
		return SellCount;
	}
	

	CModel* GetModel() 
	{
		return mp_col_model;
	}
	CModel* GetNavModel()const {
		return mp_colNavModel;
	}
};