#pragma once
#include "UIBase.h"
#include "SceneManager.h"
class UI {
private:

    static void StaticFunc() {
        printf("UIのStaticFunc呼び出し\n");
    }
    void Func() {
        printf("UIのFunc呼び出し\n");
    }

    UIBase* m_ui_frame;
    float m_member_float;
    float* m_sunset; // 参照用ポインタで sunset を管理
    float* m_soundvolume;

    bool m_isFreeRoaming; // カメラ切り替え状態を保持

    static UI* ms_instance; // 静的インスタンスへのポインタ

public:
    UI(float* sunset, float* soundvolume);
    ~UI();

    void Draw();

    void ToggleFreeRoaming() 
    {
        m_isFreeRoaming = !m_isFreeRoaming;
    }
    static UI* Instance(); // 静的インスタンスを取得
    bool IsFreeRoaming() const { return m_isFreeRoaming; } // フリーローミング状態を返す
};