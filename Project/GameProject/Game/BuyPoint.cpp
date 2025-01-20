#include"BuyPoint.h"
#include "UIBase.h"
#include "Weapon.h"
#include "Player.h"
#include "Item.h"
#include "GreenFilter.h"

//必須（imguiフォルダのソースファイルすべて）
//「imgui」を検索し参照
//https://github.com/ocornut/imgui
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl2.h"
#include "../imgui/imgui_ja_gryph_ranges.h"


BuyPoint::BuyPoint(const CVector3D& pos) :Base(eBuypoint)
{
	m_rad = 0.6f;
	m_pos = pos;
	count = 0;
	Speedcount = 0;
	buyflag = false;


	//まずはUIFrameを生成
	m_ui_frame = new UIFrame(u8"Frame");
	//UIFrameの子にUIWindow生成
	UIBase* window = new UIWindow(m_ui_frame, u8"Option", CVector2D(0, 0), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	//各種UI部品はwindowの子として生成
	//new UISliderFloat(window, u8"SliderFloat", &m_member_float, 0.1f, 5.0f);

	
	new UIButton(window, u8"剣　購入50円", []() {
		
		if (Player::Instance()->GetPlayerMoney() >= -100) {
			Base* b = Base::FindObject(ePlayer);
			Player* player = dynamic_cast<Player*>(b);
			//printf("kounyuu\n");
					//50円支払う
			player->Pay(50);
			//PlayerMoneyMax -= 50;

			//新しい武器を生成
			Base::Add(new Weapon(false, CVector3D(2, -1.0, 0), 15.0f));
		}
		});

	new UIButton(window, u8"回復　購入100円", []() {
		if (Player::Instance()->GetPlayerMoney() >= 100) {
			Base* b = Base::FindObject(ePlayer);
			Player* player = dynamic_cast<Player*>(b);
			if (player->GetHp() <= 100)
			{
				player->Pay(100);
				GreenFilter* g = dynamic_cast<GreenFilter*>(Base::FindObject(eRedFilter));
				if (g == __nullptr)
					Base::Add(g = new GreenFilter());
				g->GreenShow(30.0f);
				player->Heal(10);
			}
		}
		});

	new UIButton(window, u8"スピードアップ　購入300円", []() {
		if (Player::Instance()->GetPlayerMoney() >= 300  /* && Speedcount <= 2*/)
		{
			Base* b = Base::FindObject(ePlayer);
			Player* player = dynamic_cast<Player*>(b);
			player->Pay(300);
			player->MovespeedUp(0.2);
			//Speedcount += 1;
		}
		});

	new UIButton(window, u8"ジェットパック　購入2000円", []() {
		if (Player::Instance()->GetPlayerMoney() >= -4000  /* && Speedcount <= 2*/)
		{
			Base* b = Base::FindObject(ePlayer);
			Player* player = dynamic_cast<Player*>(b);
			player->Pay(2000);
			Base::Add(new Item(false, CVector3D(2, -1.0, 0), 30));
		}
		});

	//new UIButton(window, u8"自身のメンバ関数", std::bind(&UI::Func, this));

}

BuyPoint::~BuyPoint()
{

}

void BuyPoint::Update()
{
	
	//CInput::ShowCursor(buyflag);

	PlayerMoneyMax = Player::Instance()->GetPlayerMoney();
	
}

void BuyPoint::Render()
{
	

	m_lineS = m_pos + CVector3D(0, 2.0f - m_rad, 0);
	m_lineE = m_pos + CVector3D(0, m_rad, 0);
	//Utility::DrawOBB(m_obb, CVector4D(1, 1, 0, 0.5));
	Utility::DrawCapsule(m_lineS, m_lineE, m_rad, CVector4D(0, 0, 1, 0.5));
}

void BuyPoint::Collision(Base* b)
{
	switch (b->GetType())
	{
	case ePlayer:
		//プレイヤーと購入地点
	
		if (Player* player = dynamic_cast<Player*>(b))
		{ 
			
			//カプセル同士の衝突
			if (CCollision::CollisionCapsule(player->m_lineS, player->m_lineE, player->m_rad,
				m_lineS, m_lineE, m_rad))
			{
				if (buyflag == false)
				{
					CInput::ShowCursor(true);
				}

				buyflag = true;
			
				//	FONT_T()->Draw(1500, 750, 1.0f, 1.0f, 1.0f, "Ekeyで剣を購入 50enn" );
				//	FONT_T()->Draw(1500, 788, 1.0f, 1.0f, 1.0f, "FkeyでHPを回復 100enn");
				//	FONT_T()->Draw(1500, 826, 1.0f, 1.0f, 1.0f, "Rkeyで素早さUP 300enn");
				//	if (count ==0)
				//	{
				//		FONT_T()->Draw(1500, 864, 1.0f, 1.0f, 1.0f, "Qkeyでジャンプ解放 1000enn");
				//	}

			}
			else
			{
				if (buyflag == true)
				{
					CInput::ShowCursor(false);
				}

				buyflag = false;
			}

		}
		break;
	}
}

void BuyPoint::Draw()
{
	
	if (!buyflag)return;
	m_ui_frame->Draw();
	return;

	
	
	/*
	//必須--------------------------------------------------------------------
	//GUIの描画の前に一度だけ呼び出す
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//Push→Pop
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.0f, 0.0f, 0.1f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.1f, 0.3f, 0.6f));

	ImGui::Begin(u8"Option", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	ImGui::LabelText(u8"剣","剣");
	ImGui::SameLine();

	ImGui::LabelText(u8"回復","回復");
	ImGui::SameLine();

	/*
	if (ImGui::Button(u8"購入100円") && PlayerMoneyMax >= 100) {
		Base* b = Base::FindObject(ePlayer);
		Player* player = dynamic_cast<Player*>(b);
		if (player->GetHp() <= 100)
				{
					player->Pay(100);
					GreenFilter* g = dynamic_cast<GreenFilter*>(Base::FindObject(eRedFilter));
					if (g == __nullptr)
						Base::Add(g = new GreenFilter());
					g->GreenShow(30.0f);
					player->Heal(10);
				}
	}*/

	if (Speedcount <= 2)
	{
	//	ImGui::LabelText(u8"スピードアップ", "スピードアップ");
		ImGui::SameLine();

		//if (ImGui::Button(u8"購入300円")  /*&& PlayerMoneyMax >= 300 || */ && Speedcount <= 2)
		//{
		//	Base* b = Base::FindObject(ePlayer);
		//	Player* player = dynamic_cast<Player*>(b);
		//	player->Pay(300);
	//		player->MovespeedUp(0.2);
		//	Speedcount += 1;
		//}
	}


	//if (count == 0)
	//{
	//	ImGui::LabelText(u8"ジャンプ解放", "ジャンプ開放");
	//	ImGui::SameLine();

	//	if (ImGui::Button(u8"購入1000円") /* && PlayerMoneyMax >= 1000*/ && count == 0)
	//	{
	//		Base* b = Base::FindObject(ePlayer);
	//		Player* player = dynamic_cast<Player*>(b);
	//		count++;
	//		player->Pay(500);
	//		player->PlayerJump(true);
	//	}
	//}

	//ImGui::LabelText(u8"ジェットパック", "ジェットパック");
	ImGui::SameLine();

	//if (ImGui::Button(u8"購入	2000円") /* && PlayerMoneyMax >= 2000*/)
	//{
	//	Base* b = Base::FindObject(ePlayer);
	//	Player* player = dynamic_cast<Player*>(b);
	//	player->Pay(2000);
	//	Base::Add(new Item(false, CVector3D(2, -1.0, 0),30));
	//}

	//if (!m_kill && (ImGui::Button(u8"戻る") || PUSH(CInput::eButton11))) {
	//
	//}
	


	ImGui::End();

	/*
	ImGui::Begin(u8"Exit", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	if (ImGui::Button(u8"ゲームをやめる"))
		glfwSetWindowShouldClose(GL::window, GL_TRUE);
	ImGui::End();
	*/

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	//必須--------------------------------------------------------------------
	// Rendering
	//１更新で一回の呼び出し
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	//---------------------------------------------------------------------------


}