#include "UI.h"
#include "Camera.h"
#include <cstdio>
#include <functional>

// 静的メンバの初期化
UI* UI::ms_instance = nullptr;

// コンストラクタ
UI::UI(float* sunset, float* soundvolume)
    : m_ui_frame(nullptr), m_member_float(0), m_sunset(sunset), m_soundvolume(soundvolume), m_isFreeRoaming(false){
    ms_instance = this;

    // UIFrameの生成
    m_ui_frame = new UIFrame(u8"Frame");

    // UIWindowの生成
    UIBase* window = new UIWindow(
        m_ui_frame, u8"Option", CVector2D(0, 0),
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
    );

    // スライダーの追加
    new UISliderFloat(window, u8"Sunset Intensity", m_sunset, 0.0f, 1.0f);
    new UISliderFloat(window, u8"Sound Intensity", m_soundvolume, 0.0f, 1.0f);

    // タイトルに戻るボタン
    new UIButton(window, u8"タイトルに戻る", []() {
        SceneManager::LoadTitle(); // タイトルに戻る処理
        });

    // カメラ切り替えボタン
    new UIButton(window, u8"カメラ切り替え", []() {
        if (UI::Instance()->IsFreeRoaming()) {
            Camera::Instance()->SetMode(Camera::CameraMode::ePlayerCamera);
            printf("通常カメラに戻しました\n");
        }
        else {
            Camera::Instance()->SetMode(Camera::CameraMode::eFreeRoam);
            printf("フリーローミングカメラを開始しました\n");
        }
        UI::Instance()->ToggleFreeRoaming(); // UIの状態も切り替える
        });

    new UIButton(window, u8"カメラを固定", []() {
        CVector3D fixedPos = Camera::Instance()->GetPos();
        CVector3D fixedRot = Camera::Instance()->GetRotation();
        Camera::Instance()->SetFixedCamera(fixedPos, fixedRot);
        printf("カメラを固定しました。\n");
        });

    // その他ボタンの例
    new UIButton(window, u8"関数、静的関数", UI::StaticFunc);
    new UIButton(window, u8"自身のメンバ関数", std::bind(&UI::Func, this));

    // 別のウインドウ
    window = new UIWindow(
        m_ui_frame, u8"Exit", CVector2D(0, 0),
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
    );
    new UIButton(window, u8"ゲームをやめる", []() {
        glfwSetWindowShouldClose(GL::window, GL_TRUE); // ゲーム終了
        });
}

// デストラクタ
UI::~UI() {
    delete m_ui_frame;
}

// UIの描画
void UI::Draw() {
    m_ui_frame->Draw();
}

// 静的インスタンス取得
UI* UI::Instance() {
    return ms_instance;
}