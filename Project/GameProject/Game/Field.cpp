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
        // チュートリアルエリアの初期化
        mp_model = GET_RESOURCE("Fieldtutorial", CModel);
        mp_model->SetScale(1.0, 1.0, 1.0);
        mp_col_model = mp_model;
        mp_col_model->UpdateMatrix();

        mp_colNavModel = GET_RESOURCE("FieldNavtutorial", CModel);
        mp_colNavModel->UpdateMatrix();

        // チュートリアル文章を設定
        std::vector<std::wstring> tutorialSteps = {
    L"やぁ！このエリアでは基本操作を学べます。",
    L"WASDキーを使ってキャラクターを動かしてみましょう！",
    L"スペースキーでジャンプしてみてください！",
    L"足元に武器があります。Eキーを押して拾いましょう！",
    L"敵が近づいてきます！左クリックで攻撃してください！",
    L"倒した敵を売却地点まで運びましょう！",
    L"素晴らしい！次のエリアへ進みましょう！"
        };

        Base::Add(new Sellpoint(CVector3D(0, -4.5, 0)));
        Base::Add(new BuyPoint(CVector3D(2, -4.5, 0)));
        Base::Add(new Weapon(false, CVector3D(0, 0, 3), 5.0f));
        Base::Add(new Enemy(CVector3D(0, 2, 15), 2, 25, Enemy::eRole_Flanker));
        Base::Add(new Spider(CVector3D(4, 0, 60), 1, 10));
        Base::Add(new Spider(CVector3D(0, 0, 60), 1, 15));

        // TutorialManagerの初期化
        tutorialManager = new TutorialManager(tutorialSteps, 0.05f); // 1文字0.05秒で表示
        tutorialManager->Start(); // チュートリアル開始
        break;
    }
    case 1: {
        // 通常エリアの初期化
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
        // ライトの追加
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
    // チュートリアルマネージャを解放
    if (tutorialManager) {
        delete tutorialManager;
        tutorialManager = nullptr;
    }
}

void Field::Update() {
    // チュートリアルマネージャの更新
    if (tutorialManager) { 
     /*
    Renderer renderer;
    // テスト用の矩形を描画
    renderer.DrawTestRect();*/

        tutorialManager->Update(CFPS::GetDeltaTime());
    }

    if (CInput::PUSH(CInput::eButton1)) { // Zキーで次のステップに進行
        if (tutorialManager && !tutorialManager->IsFinished()) { // 終了していない場合のみ進行
            tutorialManager->NextStep();
        }
    }


}

void Field::Render() {
    // フィールドモデルの描画
    mp_model->Render();

    // チュートリアルマネージャの描画（終了していない場合のみ）
    if (tutorialManager && !tutorialManager->IsFinished()) {
        tutorialManager->Render();
    }
}

void Field::IncrementSellCount() 
{
    SellCount++;
}

