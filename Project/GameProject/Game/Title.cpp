#include "Title.h"
#include "Game.h"
#include "SceneManager.h"
#include "BlackFilter.h"
#include "Tutorial/Renderer.h"

#include <algorithm>

//�K�{�iimgui�t�H���_�̃\�[�X�t�@�C�����ׂāj
//�uimgui�v���������Q��
//https://github.com/ocornut/imgui
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl2.h"
#include "../imgui/imgui_ja_gryph_ranges.h"


Title::Title() : Base(eTitle),m_displayProgress(0.0f)
{
    // ����̏�����
    m_video = new CVideo("UI/TitleInternal.mp4");
    m_video->Play(true);
    m_step = 0;
    m_select = 0;

    while (ShowCursor(true) < 0);
    CInput::ShowCursor(true);
    SOUND("�^�C�g�����y")->Play(true);

}

int Title::m_select = 0;

Title::~Title()
{
    // ���惊�\�[�X�̉��
    if (m_video)
    {
        delete m_video;
        m_video = nullptr;
    }

    
}

void Title::Update()
{
    /*
    // �Q�[���J�n�{�^���������̏���
    if (PUSH(CInput::eButton2))
    {
        KillALL();
        BlackFilter* b = dynamic_cast<BlackFilter*>(Base::FindObject(eRedFilter));
        if (b == nullptr)
        {
            b = new BlackFilter();
            Base::Add(b);
        }
        b->BlackShow(120.0f);
    }*/

    if (m_step == 1) {
        // ���̉�ʂɑJ�ڂ���ۂɃJ�[�\�����\����
        if (m_showCursor) {
            while (ShowCursor(false) >= 0);
            CInput::ShowCursor(false);  // �J�[�\�����\��
            m_showCursor = false;  // �t���O���X�V

        }
    }
}

void Title::Draw()
{
    // �����`��
    m_video->Draw();

    switch (m_step)
    {

        if (!m_showCursor) {
            // �J�[�\����\��
            while (ShowCursor(true) < 0);
            CInput::ShowCursor(true);
            m_showCursor = true;  // �t���O���X�V
        }

    case 0:
        // ImGui�V�����t���[���̊J�n
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // �^�C�g���E�B���h�E
        ImGui::SetNextWindowPos(ImVec2(800, 700), ImGuiCond_Always); // �ʒu���Œ�
        ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Always); // �T�C�Y���Œ�
        ImGui::Begin("", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse| ImGuiWindowFlags_NoScrollbar| ImGuiWindowFlags_NoTitleBar);
        

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.0f, 0.2f, 0.7f)); // �Â���
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.0f, 0.5f, 0.8f)); // ���邢��
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.0f, 0.8f, 0.9f)); // �A�N�e�B�u��

        // �^�C�g���e�L�X�g
        ImGui::Text("Welcome to the Game!");
        ImGui::Separator();

        // �Q�[���J�n�{�^��
        if (ImGui::Button("Start Game", ImVec2(200, 50))) // ��200�A����50
        {

            // �J�ڎ��ɃJ�[�\�����\��
            while (ShowCursor(false) >= 0);
            CInput::ShowCursor(false);
            m_showCursor = false;  // �t���O���X�V

            m_select = 1;
            m_step = 1;

        }

        if (ImGui::Button("Tutorial", ImVec2(200, 50)))
        {

            // �J�ڎ��ɃJ�[�\�����\��
            while (ShowCursor(false) >= 0);
            CInput::ShowCursor(false);
            m_showCursor = false;  // �t���O���X�V

            m_select = 0;
            m_step = 1;
            
        }

        // �I���{�^��
        if (ImGui::Button("Exit", ImVec2(200, 50)))
        {
            exit(0);
        }

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::End();

        // ImGui�̕`��
        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        break;
    case 1:
        if (!Base::FindObject(eRedFilter))
        {
            Base::Add(new BlackFilter(120.0f));
        }
        CLoadThread::GetInstance()->LoadStart([]() {
            switch (Title::m_select)
            {
            case 0:
                ADD_RESOURCE("Fieldtutorial", CModel::CreateModel("Field/Map/mapsin.obj", 96, 32, 96));
                ADD_RESOURCE("FieldNavtutorial", CModel::CreateModel("Field/Map/mapnav.obj", 16, 1, 16));
                break;
            case 1:

                ADD_RESOURCE("Field", CModel::CreateModel("Field/Dungeon/Dungeon2.obj", 96, 32, 96));
                ADD_RESOURCE("FieldNav", CModel::CreateModel("Field/Map/mapnav.obj", 16, 1, 16));
                break;
            }
            });
        m_step++;
        break;
    case 2:
    {
        // ���ۂ̐i�������擾
        float actualProgress = GetLoadingProgress();

        if (m_displayProgress < 0.5f)
        {
            // �t�F�C�N�i���o�[�̐i�s���x
            float fakeIncrementSpeed = 0.001f;

            // �t�F�C�N�i���o�[�����X�ɐi�߂�
            m_displayProgress += fakeIncrementSpeed;
        }

        // �t�F�C�N�i���̒ǂ������o*
        if (m_displayProgress < actualProgress) {
            // �ŏ��̓X���[�Y�ɐi�s
            if (m_displayProgress < 0.5f) {
                m_displayProgress += (actualProgress - m_displayProgress) * 0.07f; // ����
            }
            // ���ՂŒ�؊������o
            else if (m_displayProgress < 0.8f) {
                m_displayProgress += (actualProgress - m_displayProgress) * 0.02f; // ��؊�
            }
            // �I�Ղł������i�s
            else {
                m_displayProgress += (actualProgress - m_displayProgress) * 0.01f; // ���ɒx��
            }
        }

        //�i���X�g�b�p�[
        m_displayProgress = (m_displayProgress < 1.0f) ? m_displayProgress : 1.0f;

        // �v���O���X�o�[�̕`��
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // �ʒu�𒆉����ɒ���
        ImGui::SetNextWindowPos(ImVec2(600, 800), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(600, 120));
        ImGui::Begin("Loading...", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);
        ImGui::Text("Loading Resources...");

        // �v���O���X�o�[��`��
        ImGui::ProgressBar(m_displayProgress, ImVec2(600.0f, 45.0f));
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        // ���ۂ̐i���ƃt�F�C�N�i�������������玟�̃X�e�b�v��
        if (actualProgress >= 1.0f && m_displayProgress >= 0.99f) {
            m_step++;
        }
    }
    break;
    case 3:
        if (CLoadThread::GetInstance()->CheckEnd()) {
            SceneManager::LoadGame(m_select);
            m_step++;
        }
        break;
    }

    if (!CLoadThread::GetInstance()->CheckEnd())
    {
        static int cnt = 0;
        cnt++;
        char str[64] = "";
        strncpy_s(str, 64, "Loading...", (cnt / 10) % 10);
        FONT_T()->Draw(0, 64, 1, 1, 1, str);

    }

}

float Title::GetLoadingProgress()
{
    if (CLoadThread::GetInstance()->CheckEnd())
    {
        return 1.0f;//���[�h����
    }
    return CLoadThread::GetInstance()->GetProgress();
}

