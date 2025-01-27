#include "Field.h"
#include "Game/Light.h"
#include "BuyPoint.h"
#include "Sellpoint.h"
#include "Tutorial/TutorialManager.h"
#include "BlackFilter.h"
#include "Tutorial/Renderer.h"
#include "Title.h"
#include "Weapon.h"
#include "Enemy.h"
#include "Spider.h"


Field::Field(int area) : Base(eField), tutorialManager(nullptr), SellCount(0), isFinished(false) {
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
    L"�����ɕ��킪����܂��BE�L�[�������ďE���܂��傤�I",
    L"�G���߂Â��Ă��܂��I���N���b�N�ōU�����Ă��������I",
    L"�|�����G�𔄋p�n�_�܂ŉ^�т܂��傤�I",
    L"�f���炵���I���̃G���A�֐i�݂܂��傤�I"
        };

        Base::Add(new Sellpoint(CVector3D(0, -4.5, 0)));
        Base::Add(new BuyPoint(CVector3D(2, -4.5, 0)));
        Base::Add(new Weapon(false, CVector3D(0, 0, 3), 5.0f));
        Base::Add(new Enemy(CVector3D(0, 2, 15), 2, 25, Enemy::eRole_Flanker));
        Base::Add(new Spider(CVector3D(4, 0, 60), 1, 10));
        Base::Add(new Spider(CVector3D(0, 0, 60), 1, 15));

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

        Base::Add(new Sellpoint(CVector3D(0, -1.0, 0)));
        Base::Add(new BuyPoint(CVector3D(2, -1.0, 0)));
        Base::Add(new Sellpoint(CVector3D(12.51, -1.0, 58.52)));
        Base::Add(new BuyPoint(CVector3D(14.51, -1.0, 58.52)));

        Base::Add(new Sellpoint(CVector3D(-6.741, 2, 128.02)));
        Base::Add(new BuyPoint(CVector3D(-6.741, 2, 130.02)));

        Base::Add(new Weapon(false, CVector3D(0, -1, 1), 5.0f));
        
        Base::Add(new Spider(CVector3D(29, 1, 28), 3, 10));
        Base::Add(new Spider(CVector3D(-16, 1, 28), 3, 10));
        Base::Add(new Spider(CVector3D(4, -1, 107), 3, 10));

        Base::Add(new Enemy(CVector3D(21, 0.8, 9), 2, 25, Enemy::eRole_Flanker));
        Base::Add(new Enemy(CVector3D(20, 0.8, 9), 2, 25, Enemy::eRole_Flanker));

        Base::Add(new Enemy(CVector3D(42.26, -1.318, 86.97), 2, 25, Enemy::eRole_Flanker));
        Base::Add(new Enemy(CVector3D(43.26, -1.318, 86.97), 2, 25, Enemy::eRole_Attacker));


        Base::Add(new Enemy(CVector3D(11.96, -1.318, 82.54), 2, 25, Enemy::eRole_Flanker));
        Base::Add(new Enemy(CVector3D(-6.77, -1.318, 71.66), 2, 25, Enemy::eRole_Flanker));

        Base::Add(new Spider(CVector3D(7.25, -1, 72.51), 3, 10));
        Base::Add(new Spider(CVector3D(18, -0.5, 49), 3, 10));

        Base::Add(new Enemy(CVector3D(30.1, -1.318, 99.96), 2, 25, Enemy::eRole_Attacker));
        Base::Add(new Enemy(CVector3D(31.1, -1.318, 99.96), 2, 25, Enemy::eRole_Attacker));
        Base::Add(new Spider(CVector3D(30, -0.5, 46), 3, 10));

        Base::Add(new Weapon(false, CVector3D(-3, 0,0), 5.0f));

        int idx = 3;
        // ���C�g�̒ǉ�
        Base::Add(new Light(CVector3D(13.46, 0, 105.8), idx++));
        Base::Add(new Light(CVector3D(-6.74, 0, 105.8), idx++));
        Base::Add(new Light(CVector3D(0.5, 3.5, 0), idx++));
        Base::Add(new Light(CVector3D(3.5, -0.3, 106), idx++));



        Base::Add(new Light(CVector3D(23.34, 5.1, 136.67), idx++));
        Base::Add(new Light(CVector3D(37.19, 5.1, 128.8), idx++));
        Base::Add(new Light(CVector3D(23.43, 5.1, 120.78), idx++));

        Base::Add(new Light(CVector3D(17.64, 0.89, 90.63), idx++));
        Base::Add(new Light(CVector3D(-11.154, 0.89, 90.63), idx++));


        Base::Add(new Light(CVector3D(-11.854, 2.119, -11.26), idx++));
        Base::Add(new Light(CVector3D(12.25, 2.311, -4.27), idx++));

        Base::Add(new Light(CVector3D(-19.33, 2.199, 32.648), idx++));
        Base::Add(new Light(CVector3D(-3.952, 2.33, 13.959), idx++));
        Base::Add(new Light(CVector3D(-0.326, 2.343, 5.957), idx++));
        Base::Add(new Light(CVector3D(7.462, 2.119, 13.96), idx++));
        Base::Add(new Light(CVector3D(-0.552, 2.734, 22.693), idx++));

        Base::Add(new Light(CVector3D(36.902, 3.267, 32.165), idx++));
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

void Field::IncrementSellCount() 
{
    SellCount++;
}

