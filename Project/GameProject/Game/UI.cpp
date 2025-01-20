#include "UI.h"
#include "Camera.h"
#include <cstdio>
#include <functional>

// �ÓI�����o�̏�����
UI* UI::ms_instance = nullptr;

// �R���X�g���N�^
UI::UI(float* sunset, float* soundvolume)
    : m_ui_frame(nullptr), m_member_float(0), m_sunset(sunset), m_soundvolume(soundvolume), m_isFreeRoaming(false){
    ms_instance = this;

    // UIFrame�̐���
    m_ui_frame = new UIFrame(u8"Frame");

    // UIWindow�̐���
    UIBase* window = new UIWindow(
        m_ui_frame, u8"Option", CVector2D(0, 0),
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
    );

    // �X���C�_�[�̒ǉ�
    new UISliderFloat(window, u8"Sunset Intensity", m_sunset, 0.0f, 1.0f);
    new UISliderFloat(window, u8"Sound Intensity", m_soundvolume, 0.0f, 1.0f);

    // �^�C�g���ɖ߂�{�^��
    new UIButton(window, u8"�^�C�g���ɖ߂�", []() {
        SceneManager::LoadTitle(); // �^�C�g���ɖ߂鏈��
        });

    // �J�����؂�ւ��{�^��
    new UIButton(window, u8"�J�����؂�ւ�", []() {
        if (UI::Instance()->IsFreeRoaming()) {
            Camera::Instance()->SetMode(Camera::CameraMode::ePlayerCamera);
            printf("�ʏ�J�����ɖ߂��܂���\n");
        }
        else {
            Camera::Instance()->SetMode(Camera::CameraMode::eFreeRoam);
            printf("�t���[���[�~���O�J�������J�n���܂���\n");
        }
        UI::Instance()->ToggleFreeRoaming(); // UI�̏�Ԃ��؂�ւ���
        });

    new UIButton(window, u8"�J�������Œ�", []() {
        CVector3D fixedPos = Camera::Instance()->GetPos();
        CVector3D fixedRot = Camera::Instance()->GetRotation();
        Camera::Instance()->SetFixedCamera(fixedPos, fixedRot);
        printf("�J�������Œ肵�܂����B\n");
        });

    // ���̑��{�^���̗�
    new UIButton(window, u8"�֐��A�ÓI�֐�", UI::StaticFunc);
    new UIButton(window, u8"���g�̃����o�֐�", std::bind(&UI::Func, this));

    // �ʂ̃E�C���h�E
    window = new UIWindow(
        m_ui_frame, u8"Exit", CVector2D(0, 0),
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
    );
    new UIButton(window, u8"�Q�[������߂�", []() {
        glfwSetWindowShouldClose(GL::window, GL_TRUE); // �Q�[���I��
        });
}

// �f�X�g���N�^
UI::~UI() {
    delete m_ui_frame;
}

// UI�̕`��
void UI::Draw() {
    m_ui_frame->Draw();
}

// �ÓI�C���X�^���X�擾
UI* UI::Instance() {
    return ms_instance;
}