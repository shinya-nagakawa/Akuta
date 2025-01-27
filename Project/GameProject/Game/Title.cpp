#include "Title.h"
#include "Game.h"
#include "SceneManager.h"
#include "BlackFilter.h"
#include "Tutorial/Renderer.h"

#include <algorithm>

//必須（imguiフォルダのソースファイルすべて）
//「imgui」を検索し参照
//https://github.com/ocornut/imgui
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl2.h"
#include "../imgui/imgui_ja_gryph_ranges.h"


Title::Title() : Base(eTitle),m_displayProgress(0.0f)
{
    // 動画の初期化
    m_video = new CVideo("UI/TitleInternal.mp4");
    m_video->Play(true);
    m_step = 0;
    m_select = 0;

    while (ShowCursor(true) < 0);
    CInput::ShowCursor(true);
    SOUND("タイトル音楽")->Play(true);

}

int Title::m_select = 0;

Title::~Title()
{
    // 動画リソースの解放
    if (m_video)
    {
        delete m_video;
        m_video = nullptr;
    }

    
}

void Title::Update()
{
    /*
    // ゲーム開始ボタン押下時の処理
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
        // 他の画面に遷移する際にカーソルを非表示に
        if (m_showCursor) {
            while (ShowCursor(false) >= 0);
            CInput::ShowCursor(false);  // カーソルを非表示
            m_showCursor = false;  // フラグを更新

        }
    }
}

void Title::Draw()
{
    // 動画を描画
    m_video->Draw();

    switch (m_step)
    {

        if (!m_showCursor) {
            // カーソルを表示
            while (ShowCursor(true) < 0);
            CInput::ShowCursor(true);
            m_showCursor = true;  // フラグを更新
        }

    case 0:
        // ImGui新しいフレームの開始
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // タイトルウィンドウ
        ImGui::SetNextWindowPos(ImVec2(800, 700), ImGuiCond_Always); // 位置を固定
        ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Always); // サイズを固定
        ImGui::Begin("", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse| ImGuiWindowFlags_NoScrollbar| ImGuiWindowFlags_NoTitleBar);
        

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.0f, 0.2f, 0.7f)); // 暗い紫
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.0f, 0.5f, 0.8f)); // 明るい紫
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.0f, 0.8f, 0.9f)); // アクティブ時

        // タイトルテキスト
        ImGui::Text("Welcome to the Game!");
        ImGui::Separator();

        // ゲーム開始ボタン
        if (ImGui::Button("Start Game", ImVec2(200, 50))) // 幅200、高さ50
        {

            // 遷移時にカーソルを非表示
            while (ShowCursor(false) >= 0);
            CInput::ShowCursor(false);
            m_showCursor = false;  // フラグを更新

            m_select = 1;
            m_step = 1;

        }

        if (ImGui::Button("Tutorial", ImVec2(200, 50)))
        {

            // 遷移時にカーソルを非表示
            while (ShowCursor(false) >= 0);
            CInput::ShowCursor(false);
            m_showCursor = false;  // フラグを更新

            m_select = 0;
            m_step = 1;
            
        }

        // 終了ボタン
        if (ImGui::Button("Exit", ImVec2(200, 50)))
        {
            exit(0);
        }

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::End();

        // ImGuiの描画
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
        // 実際の進捗率を取得
        float actualProgress = GetLoadingProgress();

        if (m_displayProgress < 0.5f)
        {
            // フェイク進捗バーの進行速度
            float fakeIncrementSpeed = 0.001f;

            // フェイク進捗バーを徐々に進める
            m_displayProgress += fakeIncrementSpeed;
        }

        // フェイク進捗の追いつき演出*
        if (m_displayProgress < actualProgress) {
            // 最初はスムーズに進行
            if (m_displayProgress < 0.5f) {
                m_displayProgress += (actualProgress - m_displayProgress) * 0.07f; // 速め
            }
            // 中盤で停滞感を演出
            else if (m_displayProgress < 0.8f) {
                m_displayProgress += (actualProgress - m_displayProgress) * 0.02f; // 停滞感
            }
            // 終盤でゆっくり進行
            else {
                m_displayProgress += (actualProgress - m_displayProgress) * 0.01f; // 非常に遅く
            }
        }

        //進捗ストッパー
        m_displayProgress = (m_displayProgress < 1.0f) ? m_displayProgress : 1.0f;

        // プログレスバーの描画
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 位置を中央下に調整
        ImGui::SetNextWindowPos(ImVec2(600, 800), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(600, 120));
        ImGui::Begin("Loading...", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);
        ImGui::Text("Loading Resources...");

        // プログレスバーを描画
        ImGui::ProgressBar(m_displayProgress, ImVec2(600.0f, 45.0f));
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        // 実際の進捗とフェイク進捗が完了したら次のステップへ
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
        return 1.0f;//ロード完了
    }
    return CLoadThread::GetInstance()->GetProgress();
}

