#pragma once
#include "UIBase.h"
#include "SceneManager.h"
class UI {
private:

    static void StaticFunc() {
        printf("UI��StaticFunc�Ăяo��\n");
    }
    void Func() {
        printf("UI��Func�Ăяo��\n");
    }

    UIBase* m_ui_frame;
    float m_member_float;
    float* m_sunset; // �Q�Ɨp�|�C���^�� sunset ���Ǘ�
    float* m_soundvolume;

    bool m_isFreeRoaming; // �J�����؂�ւ���Ԃ�ێ�

    static UI* ms_instance; // �ÓI�C���X�^���X�ւ̃|�C���^

public:
    UI(float* sunset, float* soundvolume);
    ~UI();

    void Draw();

    void ToggleFreeRoaming() 
    {
        m_isFreeRoaming = !m_isFreeRoaming;
    }
    static UI* Instance(); // �ÓI�C���X�^���X���擾
    bool IsFreeRoaming() const { return m_isFreeRoaming; } // �t���[���[�~���O��Ԃ�Ԃ�
};