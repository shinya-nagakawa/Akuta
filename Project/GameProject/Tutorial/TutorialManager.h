#pragma once


class TutorialManager {
private:
    std::vector<std::wstring> m_steps; // ���{��Ή��̃��C�h������
    int m_currentStep;                // ���݂̃X�e�b�v
    bool m_isActive;                  // �`���[�g���A�����A�N�e�B�u���ǂ���
    float m_charDisplaySpeed;         // 1������\�����鑬�x�i�b�j
    float m_elapsedTime;              // �o�ߎ���
    int m_charCount;
    std::wstring m_displayedText; // ���ݕ\�����̃e�L�X�g
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

    void Start();                  // �`���[�g���A���J�n
    void SetSellCountCondition(int requiredSellCount);
    void Update(float deltaTime);  // ���t���[���X�V
    void Render();                 // ���t���[���`��
    void NextStep();               // ���̃X�e�b�v�ɐi��
    bool IsFinished() const;       // �`���[�g���A�����I���������m�F
    void ForceDisplayMessage(const std::wstring& message);
};