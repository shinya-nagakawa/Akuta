#include "Field.h"
#include "Game/Light.h"
#include "BuyPoint.h"
#include "Sellpoint.h"
#include "Tutorial/TutorialManager.h"
#include "Tutorial/Renderer.h"

Field::Field(int area) : Base(eField), tutorialManager(nullptr) {
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
            L"お疲れ様でした！次のエリアへ進みましょう！"
        };

        Base::Add(new Sellpoint(CVector3D(0, -5.0, 0)));
        Base::Add(new BuyPoint(CVector3D(2, -5.0, 0)));

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

        // ライトの追加
        Base::Add(new Light(CVector3D(13.46, 0, 105.8), 3));
        Base::Add(new Light(CVector3D(-6.74, 0, 105.8), 4));
        Base::Add(new Light(CVector3D(0.5, 3.5, 0), 5));
        Base::Add(new Light(CVector3D(3.5, -0.3, 106), 6));
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

