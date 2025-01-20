#include"BuyPoint.h"
#include "UIBase.h"
#include "Weapon.h"
#include "Player.h"
#include "Item.h"
#include "GreenFilter.h"

//�K�{�iimgui�t�H���_�̃\�[�X�t�@�C�����ׂāj
//�uimgui�v���������Q��
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


	//�܂���UIFrame�𐶐�
	m_ui_frame = new UIFrame(u8"Frame");
	//UIFrame�̎q��UIWindow����
	UIBase* window = new UIWindow(m_ui_frame, u8"Option", CVector2D(0, 0), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	//�e��UI���i��window�̎q�Ƃ��Đ���
	//new UISliderFloat(window, u8"SliderFloat", &m_member_float, 0.1f, 5.0f);

	
	new UIButton(window, u8"���@�w��50�~", []() {
		
		if (Player::Instance()->GetPlayerMoney() >= -100) {
			Base* b = Base::FindObject(ePlayer);
			Player* player = dynamic_cast<Player*>(b);
			//printf("kounyuu\n");
					//50�~�x����
			player->Pay(50);
			//PlayerMoneyMax -= 50;

			//�V��������𐶐�
			Base::Add(new Weapon(false, CVector3D(2, -1.0, 0), 15.0f));
		}
		});

	new UIButton(window, u8"�񕜁@�w��100�~", []() {
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

	new UIButton(window, u8"�X�s�[�h�A�b�v�@�w��300�~", []() {
		if (Player::Instance()->GetPlayerMoney() >= 300  /* && Speedcount <= 2*/)
		{
			Base* b = Base::FindObject(ePlayer);
			Player* player = dynamic_cast<Player*>(b);
			player->Pay(300);
			player->MovespeedUp(0.2);
			//Speedcount += 1;
		}
		});

	new UIButton(window, u8"�W�F�b�g�p�b�N�@�w��2000�~", []() {
		if (Player::Instance()->GetPlayerMoney() >= -4000  /* && Speedcount <= 2*/)
		{
			Base* b = Base::FindObject(ePlayer);
			Player* player = dynamic_cast<Player*>(b);
			player->Pay(2000);
			Base::Add(new Item(false, CVector3D(2, -1.0, 0), 30));
		}
		});

	//new UIButton(window, u8"���g�̃����o�֐�", std::bind(&UI::Func, this));

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
		//�v���C���[�ƍw���n�_
	
		if (Player* player = dynamic_cast<Player*>(b))
		{ 
			
			//�J�v�Z�����m�̏Փ�
			if (CCollision::CollisionCapsule(player->m_lineS, player->m_lineE, player->m_rad,
				m_lineS, m_lineE, m_rad))
			{
				if (buyflag == false)
				{
					CInput::ShowCursor(true);
				}

				buyflag = true;
			
				//	FONT_T()->Draw(1500, 750, 1.0f, 1.0f, 1.0f, "Ekey�Ō����w�� 50enn" );
				//	FONT_T()->Draw(1500, 788, 1.0f, 1.0f, 1.0f, "Fkey��HP���� 100enn");
				//	FONT_T()->Draw(1500, 826, 1.0f, 1.0f, 1.0f, "Rkey�őf����UP 300enn");
				//	if (count ==0)
				//	{
				//		FONT_T()->Draw(1500, 864, 1.0f, 1.0f, 1.0f, "Qkey�ŃW�����v��� 1000enn");
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
	//�K�{--------------------------------------------------------------------
	//GUI�̕`��̑O�Ɉ�x�����Ăяo��
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//Push��Pop
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.0f, 0.0f, 0.1f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.1f, 0.3f, 0.6f));

	ImGui::Begin(u8"Option", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	ImGui::LabelText(u8"��","��");
	ImGui::SameLine();

	ImGui::LabelText(u8"��","��");
	ImGui::SameLine();

	/*
	if (ImGui::Button(u8"�w��100�~") && PlayerMoneyMax >= 100) {
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
	//	ImGui::LabelText(u8"�X�s�[�h�A�b�v", "�X�s�[�h�A�b�v");
		ImGui::SameLine();

		//if (ImGui::Button(u8"�w��300�~")  /*&& PlayerMoneyMax >= 300 || */ && Speedcount <= 2)
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
	//	ImGui::LabelText(u8"�W�����v���", "�W�����v�J��");
	//	ImGui::SameLine();

	//	if (ImGui::Button(u8"�w��1000�~") /* && PlayerMoneyMax >= 1000*/ && count == 0)
	//	{
	//		Base* b = Base::FindObject(ePlayer);
	//		Player* player = dynamic_cast<Player*>(b);
	//		count++;
	//		player->Pay(500);
	//		player->PlayerJump(true);
	//	}
	//}

	//ImGui::LabelText(u8"�W�F�b�g�p�b�N", "�W�F�b�g�p�b�N");
	ImGui::SameLine();

	//if (ImGui::Button(u8"�w��	2000�~") /* && PlayerMoneyMax >= 2000*/)
	//{
	//	Base* b = Base::FindObject(ePlayer);
	//	Player* player = dynamic_cast<Player*>(b);
	//	player->Pay(2000);
	//	Base::Add(new Item(false, CVector3D(2, -1.0, 0),30));
	//}

	//if (!m_kill && (ImGui::Button(u8"�߂�") || PUSH(CInput::eButton11))) {
	//
	//}
	


	ImGui::End();

	/*
	ImGui::Begin(u8"Exit", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	if (ImGui::Button(u8"�Q�[������߂�"))
		glfwSetWindowShouldClose(GL::window, GL_TRUE);
	ImGui::End();
	*/

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	//�K�{--------------------------------------------------------------------
	// Rendering
	//�P�X�V�ň��̌Ăяo��
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	//---------------------------------------------------------------------------


}