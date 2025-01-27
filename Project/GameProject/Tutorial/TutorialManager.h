#pragma once


class TutorialManager {
private:
    std::vector<std::wstring> m_steps; // 日本語対応のワイド文字列
    int m_currentStep;                // 現在のステップ
    bool m_isActive;                  // チュートリアルがアクティブかどうか
    float m_charDisplaySpeed;         // 1文字を表示する速度（秒）
    float m_elapsedTime;              // 経過時間
    int m_charCount;
    std::wstring m_displayedText; // 現在表示中のテキスト
    std::wstring m_face;
    std::vector<std::wstring>m_facePatterns;
    int m_faceIndex;
    float m_faceTimer;
    float m_faceSwitchSpeed;
    bool m_windowVisible;
    float m_windowDelay;
    float m_elapsedWindowTime;
    bool m_isTyping;
    bool m_isSlidingIn;
    bool m_isSlidingOut;
    float m_slideProgress;
    float m_slideDuration;
    bool m_waitForSellCount;
    int m_requiredSellCount;
    int currentSellCount;

public:
    TutorialManager(const std::vector<std::wstring>& steps, float charDisplaySpeed);

    void Start();                  // チュートリアル開始
    void SetSellCountCondition(int requiredSellCount);
    void Update(float deltaTime);  // 毎フレーム更新
    void Render();                 // 毎フレーム描画
    void NextStep();               // 次のステップに進む
    bool IsFinished() const;       // チュートリアルが終了したか確認
    void ForceDisplayMessage(const std::wstring& message);
};