#include "UIBase.h"

//必須（imguiフォルダのソースファイルすべて）
//「imgui」を検索し参照
//https://github.com/ocornut/imgui
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl2.h"
#include "../imgui/imgui_ja_gryph_ranges.h"

UIBase::UIBase()
{
}

UIBase::UIBase(std::string name) :m_pos(0, 0), m_name(name)
{
}

UIBase::~UIBase()
{
	//子ノード全て破棄
	for (auto& c : m_childs)
		delete c;
	m_childs.clear();
}


void UIBase::Draw()
{
	for (auto& c : m_childs)
		c->Draw();
}

void UIBase::AddChild(UIBase* childs)
{
	m_childs.push_back(childs);
}

UIFrame::UIFrame(std::string name) :UIBase(name)
{


}


void UIFrame::Draw()
{
	//必須--------------------------------------------------------------------
	//GUIの描画の前に一度だけ呼び出す
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//Push→Pop
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.0f, 0.0f, 0.1f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.1f, 0.3f, 0.6f));


	//子ノードの描画
	UIBase::Draw();

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	//必須--------------------------------------------------------------------
	// Rendering
	//１更新で一回の呼び出し
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	//---------------------------------------------------------------------------
}

UIWindow::UIWindow(UIBase* parent, std::string name, const CVector2D& pos, ImGuiWindowFlags flag) :UIBase(name)
{
	m_pos = pos;
	m_window_flag = flag;
	if (parent)parent->AddChild(this);
}

void UIWindow::Draw()
{
	ImGui::Begin(m_name.c_str(), nullptr, m_window_flag);
	UIBase::Draw();
	ImGui::End();

}

UISliderFloat::UISliderFloat(UIBase* parent, std::string name, float* v, float min, float max) :UIBase(name)
, m_out(v), m_min(min), m_max(max) {

	if (parent)parent->AddChild(this);
}
void UISliderFloat::Draw() {

	ImGui::SliderFloat(m_name.c_str(), m_out, m_min, m_max);
	UIBase::Draw();
}
UIButton::UIButton(UIBase* parent, std::string name, std::function<void()> callback) :UIBase(name)
, m_on_push(callback)
{
	if (parent)parent->AddChild(this);
}


void UIButton::Draw()
{
	if (ImGui::Button(m_name.c_str())) {
		m_on_push();
	}
	UIBase::Draw();
}
