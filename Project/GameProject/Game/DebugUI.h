#pragma once
#include "UIBase.h"

class DebugUI {
private:
    UIBase* m_ui_frame;
    float* m_sunset; // 参照用ポインタで sunset を管理
public:
    DebugUI(float* sunset) : m_ui_frame(nullptr), m_sunset(sunset) {
        m_ui_frame = new UIFrame(u8"Debug Menu");

        // "Sunset Intensity" スライダーを作成
        UIBase* window = new UIWindow(m_ui_frame, u8"Settings", CVector2D(0, 0),
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        new UISliderFloat(window, u8"Sunset Intensity", m_sunset, 0.0f, 1.0f);

        // ゲーム終了ボタン
        window = new UIWindow(m_ui_frame, u8"Exit", CVector2D(0, 0),
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        new UIButton(window, u8"Quit Game", []() {
            glfwSetWindowShouldClose(GL::window, GL_TRUE);
            });
    }

    ~DebugUI() {
        delete m_ui_frame;
    }

    void Draw() {
        m_ui_frame->Draw();
    }
};