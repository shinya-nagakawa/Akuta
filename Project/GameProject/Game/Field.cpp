#include "Field.h"
#include "Game/Light.h"
#include "BuyPoint.h"
#include "Sellpoint.h"
#include "Tutorial/TutorialManager.h"
#include "Tutorial/Renderer.h"

Field::Field(int area) : Base(eField), tutorialManager(nullptr) {
    switch (area) {
    case 0: {
        // �`���[�g���A���G���A�̏�����
        mp_model = GET_RESOURCE("Fieldtutorial", CModel);
        mp_model->SetScale(1.0, 1.0, 1.0);
        mp_col_model = mp_model;
        mp_col_model->UpdateMatrix();

        mp_colNavModel = GET_RESOURCE("FieldNavtutorial", CModel);
        mp_colNavModel->UpdateMatrix();

        // �`���[�g���A�����͂�ݒ�
        std::vector<std::wstring> tutorialSteps = {
            L"�₟�I���̃G���A�ł͊�{������w�ׂ܂��B",
            L"WASD�L�[���g���ăL�����N�^�[�𓮂����Ă݂܂��傤�I",
            L"�X�y�[�X�L�[�ŃW�����v���Ă݂Ă��������I",
            L"�����l�ł����I���̃G���A�֐i�݂܂��傤�I"
        };

        Base::Add(new Sellpoint(CVector3D(0, -5.0, 0)));
        Base::Add(new BuyPoint(CVector3D(2, -5.0, 0)));

        // TutorialManager�̏�����
        tutorialManager = new TutorialManager(tutorialSteps, 0.05f); // 1����0.05�b�ŕ\��
        tutorialManager->Start(); // �`���[�g���A���J�n
        break;
    }
    case 1: {
        // �ʏ�G���A�̏�����
        mp_model = GET_RESOURCE("Field", CModel);
        mp_model->SetScale(1.0, 1.0, 1.0);
        mp_col_model = mp_model;
        mp_col_model->UpdateMatrix();

        mp_colNavModel = GET_RESOURCE("FieldNav", CModel);
        mp_colNavModel->UpdateMatrix();

        // ���C�g�̒ǉ�
        Base::Add(new Light(CVector3D(13.46, 0, 105.8), 3));
        Base::Add(new Light(CVector3D(-6.74, 0, 105.8), 4));
        Base::Add(new Light(CVector3D(0.5, 3.5, 0), 5));
        Base::Add(new Light(CVector3D(3.5, -0.3, 106), 6));
        break;
    }
    }
}

Field::~Field() {
    // �`���[�g���A���}�l�[�W�������
    if (tutorialManager) {
        delete tutorialManager;
        tutorialManager = nullptr;
    }
}

void Field::Update() {
    // �`���[�g���A���}�l�[�W���̍X�V
    if (tutorialManager) { 
     /*
    Renderer renderer;
    // �e�X�g�p�̋�`��`��
    renderer.DrawTestRect();*/

        tutorialManager->Update(CFPS::GetDeltaTime());
    }

    if (CInput::PUSH(CInput::eButton1)) { // Z�L�[�Ŏ��̃X�e�b�v�ɐi�s
        if (tutorialManager && !tutorialManager->IsFinished()) { // �I�����Ă��Ȃ��ꍇ�̂ݐi�s
            tutorialManager->NextStep();
        }
    }

   
}

void Field::Render() {
    // �t�B�[���h���f���̕`��
    mp_model->Render();

    // �`���[�g���A���}�l�[�W���̕`��i�I�����Ă��Ȃ��ꍇ�̂݁j
    if (tutorialManager && !tutorialManager->IsFinished()) {
        tutorialManager->Render();
    }
}

