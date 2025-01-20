
#include "Field.h"
#include "Player.h"
#include "Weapon.h"
#include "Item.h"
#include "Camera.h"
#include "Enemy.h"
#include "Spider.h"
#include "GreenDragon.h"
#include "Sellpoint.h"
#include "BuyPoint.h"
#include "Game.h"
#include "Title.h"
#include "Effekseer/EffekseerManager.h"
#include "Game/NavEdit.h"
#include "Game/Navmanager.h"
#include "UI.h"
#include "Game/DebugUI.h"

//#define NAV_EDIT_MODE
UI* ui = nullptr;

Game::Game(int FieldId) :Base(eScene)
{
	m_img = COPY_RESOURCE("GameClear", CImage);
	m_img.SetSize(2000,1100);
	OpenMenu = false;
	sunset = 1.1f;
	SoundVolume = 1.0f;
	UI* ui = new UI(&sunset, &SoundVolume);

	glClearColor(0.0 * sunset, 0.0 * sunset, 0.0 * sunset, 1.0);

	//m_img.SetCenter();

	
	//Base::Add(new Player(CVector3D(3.5, -1.3, 103)));
	Base::Add(new Player(CVector3D(0,0,0)));
	Base::Add(new Spider(CVector3D(4, 0, 60),1, 10));
	Base::Add(new Spider(CVector3D(0, 0, 60), 1, 15));
	//Base::Add(new Spider(CVector3D(0, 0, 0), 1, 10));
	Base::Add(new Enemy(CVector3D(0, 2, 15),3,25,Enemy::eRole_Attacker));
	Base::Add(new Enemy(CVector3D(0, 2, 15), 2, 25, Enemy::eRole_Flanker));
	//Base::Add(new Spider(CVector3D(1, 2, 4), 3, 10));
	//Base::Add(new Spider(CVector3D(1, 0, 25), 3, 10));
	//Base::Add(new GreenDragon(CVector3D(0, 0, 0),3,10,0.04f));
	//Base::Add(new Enemy(CVector3D(21, 0.8, 9),1,20));
	//Base::Add(new Enemy(CVector3D(21, 0.8, 9), 1, 20));
	//Base::Add(new Enemy(CVector3D(0.4, 0.5, 11),5,20));
	Base::Add(new Spider(CVector3D(29, 1, 28), 3, 10));
	Base::Add(new Spider(CVector3D(29, 1, 31), 3, 10));
	//Base::Add(new Enemy(CVector3D(-40,-1, 58), 5, 20));
	//Base::Add(new Spider(CVector3D(-40, -1, 58),3,10));
	Base::Add(new Spider(CVector3D(4, -1, 107), 3, 10));
	//Base::Add(new Enemy(CVector3D(7, -1, 107), 5, 20));
	//Base::Add(new Enemy(CVector3D(10, -1, 107), 5, 20));
	//Base::Add(new Enemy(CVector3D(5, -1, 107), 5, 20,Enemy::eRole_Attacker));
	//Base::Add(new Enemy(CVector3D(4, -1, 107), 5, 20));
	

	//Base::Add(new GreenDragon(CVector3D(0, 0, 0), 3, 10,0.04f));
	Base::Add(new GreenDragon(CVector3D(3.5, -1.3, 106), 3, 10, 0.04f));

	//Base::Add(new Enemy(CVector3D(0, 2, 35),2, 20));

	//Base::Add(new Enemy(CVector3D(0, 2, 50),5, 20));
	//Base::Add(new Enemy(CVector3D(0, 2, 60),5, 20));
	//Base::Add(new Enemy(CVector3D(-20, 2, 60),5, 20));
	//Base::Add(new Enemy(CVector3D(-30, 2, 60),5, 20));
	//Base::Add(new Enemy(CVector3D(-50, 2, 30),5, 20));
	//Base::Add(new Enemy(CVector3D(-20, 2, 10),5, 20));

	/*
	Base::Add(new Enemy(CVector3D(-74.622124, 2.458017, 39.656399),5, 20));
	Base::Add(new Enemy(CVector3D(-81.240852, 0.168247, 44.605518
	),5, 20));
	Base::Add(new Enemy(CVector3D(-30.986040, -1.140389, 34.011845),5, 20));
	Base::Add(new Enemy(CVector3D(-44.287731, -0.065449, 37.407162
	),5, 20));
	Base::Add(new Enemy(CVector3D(-59.488903, 0.300692, 43.004154
	),5, 20));
	Base::Add(new Enemy(CVector3D(-51.239445, -0.033387, 21.382809),5, 20));
	Base::Add(new Enemy(CVector3D(-81.536591, 0.241746, 31.737028),5, 20));
	Base::Add(new Enemy(CVector3D(-79.510712, 0.168247, 35.478909
	),5, 20));
	Base::Add(new Enemy(CVector3D(-76.404968, 0.300559, 40.618176),5, 20));
*/

	Base::Add(new Camera());
	Base::Add(new Weapon(false, CVector3D(0, -1, 0),5.0f));
	Base::Add(new Weapon(false, CVector3D(3.5, -1.3, 103), 5.0f));
	Base::Add(new Weapon(false, CVector3D(0, -3.8, 0),20.0f));
	Base::Add(new Item(false, CVector3D(0, -3.8, -2),30));
	Base::Add(new Sellpoint(CVector3D(0, -1.0, 0)));
	Base::Add(new BuyPoint(CVector3D(2, -1.0, 0)));
	Base::Add(new Field(FieldId));

	CInput::ShowCursor(false);

	// 経路探索用のノードを作成
	NavManager::Instance()->ReadNavNodes("NavNodes.txt");
#ifdef NAV_EDIT_MODE
	CLight::SetColor(0, CVector3D(0.4f, 0.4f, 0.4f), CVector3D(0.6f, 0.6f, 0.6f));
	Base::Add(new NavEdit());
	CInput::ShowCursor(true);
#else
	//Base::Add(new EffekseerManager());
#endif

	SOUND("タイトル音楽")->Stop();
	SOUND("音楽")->Play(true);

}

Game::~Game()
{

}

void Game::Update()
{

	if (HOLD(CInput::eButton10)) {
		sunset = max(sunset - 0.001f, 0);
	}

	sunset = max(sunset - 0.000005f, 0);

	glClearColor(0.0 * sunset, 0.1 * sunset, 0.1 * sunset, 1.0);

	//ライト設定
	CLight::SetType(2, CLight::eLight_Direction);
	CLight::SetPos(2, CVector3D(0, 200, 200));
	CLight::SetDir(2, CVector3D(0.5, -1, 0.5).GetNormalize());
	CLight::SetColor(2, CVector3D(0.4f * sunset, 0.4f * sunset, 0.4f * sunset), CVector3D(0.6f * sunset, 0.6f * sunset, 0.6f * sunset));
	//CLight::SetColor(1, CVector3D(0.4f, 0.4f, 0.4f), CVector3D(0.6f, 0.6f, 0.6f));

	SOUND("音楽")->Volume(SoundVolume);

if (ui)
		ui->Draw();

	if (PUSH(CInput::eButton10)) {
		OpenMenu = true;
		if (ui) {
			delete ui;
			ui = nullptr;
			OpenMenu = false;
		}
		else {
			ui = new UI(&sunset,&SoundVolume);
		}
	}
	//FONT_T()->Draw(0, 64, 1, 0, 0, "Enterキーでメニューの生成・破棄");
}

void Game::Draw()
{
	
	if (!Base::FindObject(eEnemy))
	{
		FONT_T()->Draw(0, 128, 1, 1, 1, "Clear");
		m_img.Draw();
	}

	NavManager::Instance()->Render();

}

