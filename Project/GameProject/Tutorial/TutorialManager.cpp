#include "TutorialManager.h"
#include "Renderer.h"


TutorialManager::TutorialManager(const std::vector<std::wstring>& steps, float charDisplaySpeed)
    : m_steps(steps), m_currentStep(0), m_isActive(false), m_charDisplaySpeed(charDisplaySpeed),
    m_charCount(0), m_elapsedTime(0.0f), m_displayedText(L""),
    m_face(L":)"), m_faceTimer(0.0f), m_faceSwitchSpeed(0.085f), m_isTyping(false), m_faceIndex(0),
    m_windowVisible(false), m_windowDelay(1.5f), m_elapsedWindowTime(0.0f),
    m_isSlidingIn(false), m_isSlidingOut(false), m_slideProgress(0.0f), m_slideDuration(1.0f) {}

void TutorialManager::Start() {
    m_isActive = true;           // チュートリアルをアクティブにする
    m_currentStep = 0;           // 最初のステップから始める
    m_charCount = 0;             // 表示文字数をリセット
    m_displayedText = L"";       // 表示中の文字列をリセット
    m_isTyping = true;           // タイピングエフェクトを開始

    // 顔の種類を設定
    m_facePatterns = { L":)", L":D", L":o", L":O", L";P", L":|" };
}

void TutorialManager::Update(float deltaTime) {


    // ディレイ中はスライドインを開始しない
    if (!m_isSlidingIn && !m_windowVisible) {
        m_elapsedWindowTime += deltaTime; // ディレイ時間を加算
        if (m_elapsedWindowTime >= m_windowDelay) {
            m_isSlidingIn = true;  // スライドイン開始
        }
        return; // 他の処理をスキップ
    }

    // ウィンドウスライドイン処理
    if (m_isSlidingIn) {
        m_slideProgress += deltaTime / m_slideDuration;
        if (m_slideProgress >= 1.0f) {
            m_slideProgress = 1.0f;
            m_isSlidingIn = false;
            m_windowVisible = true;
            m_isTyping = true;
        }
        return; // スライドイン中は他の処理をスキップ
    }

    // ウィンドウスライドアウト処理
    if (m_isSlidingOut) {
        m_slideProgress -= deltaTime / m_slideDuration; // 進行度を減少
        if (m_slideProgress <= 0.0f) { // 完了
            m_slideProgress = 0.0f;
            m_isSlidingOut = false;
            m_windowVisible = false; // ウィンドウ非表示
            m_isActive = false;      // チュートリアル終了
        } 
        SOUND("声")->Stop();
        return; // スライドアウト中は他の処理をスキップ
    }

        // タイピングエフェクト処理
        if (m_charCount < m_steps[m_currentStep].size()) {
            m_elapsedTime += deltaTime;
            if (m_elapsedTime >= m_charDisplaySpeed) {
                m_charCount++;
                m_displayedText = m_steps[m_currentStep].substr(0, m_charCount);
                m_elapsedTime = 0.0f;

                // タイピング音の再生
                SOUND("声")->Play(false);
            }
            m_isTyping = true;
        }
        else {
            m_isTyping = false;
            SOUND("声")->Stop();
        }
       

        // 喋っている表情の切り替え
        if (m_isTyping) {
            m_faceTimer += deltaTime;
            if (m_faceTimer >= m_faceSwitchSpeed) {
                m_faceIndex = rand() % m_facePatterns.size();
                m_face = m_facePatterns[m_faceIndex];
                m_faceTimer = 0.0f;
            }
        }
        else {
            m_face = L":)"; // タイピングが終わったら笑顔
        }


    // 顔の切り替えアニメーション
    if (m_isTyping) { // タイピング中のみ顔を動かす
        m_faceTimer += deltaTime;
        if (m_faceTimer >= m_faceSwitchSpeed) {
            m_faceIndex = rand() % m_facePatterns.size();
            m_face = m_facePatterns[m_faceIndex];
            m_faceTimer = 0.0f; // タイマーをリセット
        }
    }
}

void TutorialManager::Render() {
    // ウィンドウの表示条件を調整
    if (!m_windowVisible && !m_isSlidingIn && !m_isSlidingOut) {
        return;
    }

    Renderer renderer;

    int windowWidth = 800;
    int windowHeight = 150;
    int paddingX = 200;
    int paddingY = 300;

    // スライドイン・アウトの進行度に応じてX座標を変更
    float startX = 1880;                     // 画面右端
    float endX = 1280 - paddingX;            // スライド後の最終位置
    int windowX = startX + (endX - startX) * m_slideProgress;
    int windowY = paddingY;

    // テキストウィンドウを描画
    renderer.DrawTextWindow(windowX, windowY, windowWidth, windowHeight, m_displayedText.c_str(),
        CVector4D(0.0f, 0.0f, 0.0f, 0.7f),
        CVector3D(1.0f, 1.0f, 1.0f),
        20);

    // 顔アイコンの描画
    int faceX = windowX + 20;
    int faceY = windowY + (windowHeight / 2) - 40;
    renderer.DrawText(m_face.c_str(), faceX, faceY, CVector3D(1.0f, 1.0f, 1.0f), 48);

    // 右上に「Zキーで進む」を描画
    int promptX = windowX + windowWidth - 130; // ウィンドウ右端より少し内側
    int promptY = windowY + 30;                // ウィンドウ上端から少し下
    renderer.DrawText(L"Zキーで進む", promptX, promptY, CVector3D(1.0f, 1.0f, 1.0f), 20); // フォントサイズ20
}

void TutorialManager::NextStep() {
    if (m_currentStep >= m_steps.size() - 1) {
        m_isSlidingOut = true; // スライドアウト開始
        return;
    }

    // 現在の文字がすべて表示されていない場合は全文を表示
    if (m_charCount < m_steps[m_currentStep].size()) {
        m_charCount = m_steps[m_currentStep].size();
        m_displayedText = m_steps[m_currentStep];
        return;
    }

    // 次のステップへ進む
    m_currentStep++;
    m_charCount = 0;          // タイピングをリセット
    m_displayedText = L"";    // 表示文字列をリセット
    m_elapsedTime = 0.0f;     // 経過時間をリセット
    m_isTyping = true;
}

bool TutorialManager::IsFinished() const {
    return !m_isActive; // アクティブでないなら完了しているとみなす
}

/*
void TutorialManager::ForceDisplayMessage(const std::wstring& message) {
    // 現在のチュートリアル状態をバックアップ
    if (!m_isActive) {
        return; // チュートリアルが終了している場合は何もしない
    }

    m_isTyping = false;           // タイピングを終了
    m_charCount = 0;              // 表示文字数をリセット
    m_displayedText = L"";        // 表示中の文字列をリセット
    m_steps = { message };        // 現在のメッセージを一時的に置き換え
    m_isSlidingIn = false;        // スライドインをリセット
    m_isSlidingOut = false;       // スライドアウトを停止
    m_windowVisible = true;       // ウィンドウを表示
    m_slideProgress = 1.0f;       // ウィンドウの表示状態を保持
}*/