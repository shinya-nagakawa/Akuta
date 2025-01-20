#include "TutorialManager.h"
#include "Renderer.h"


TutorialManager::TutorialManager(const std::vector<std::wstring>& steps, float charDisplaySpeed)
    : m_steps(steps), m_currentStep(0), m_isActive(false), m_charDisplaySpeed(charDisplaySpeed),
    m_charCount(0), m_elapsedTime(0.0f), m_displayedText(L""),
    m_face(L":)"), m_faceTimer(0.0f), m_faceSwitchSpeed(0.045f), m_isTyping(false), m_faceIndex(0),
    m_windowVisible(false), m_windowDelay(1.5f), m_elapsedWindowTime(0.0f),
    m_isSlidingIn(false), m_isSlidingOut(false), m_slideProgress(0.0f), m_slideDuration(1.0f) {}

void TutorialManager::Start() {
    m_isActive = true;           // �`���[�g���A�����A�N�e�B�u�ɂ���
    m_currentStep = 0;           // �ŏ��̃X�e�b�v����n�߂�
    m_charCount = 0;             // �\�������������Z�b�g
    m_displayedText = L"";       // �\�����̕���������Z�b�g
    m_isTyping = true;           // �^�C�s���O�G�t�F�N�g���J�n

    // ��̎�ނ�ݒ�
    m_facePatterns = { L":)", L":D", L":o", L":O", L";P", L":|" };
}

void TutorialManager::Update(float deltaTime) {
    // �f�B���C���̓X���C�h�C�����J�n���Ȃ�
    if (!m_isSlidingIn && !m_windowVisible) {
        m_elapsedWindowTime += deltaTime; // �f�B���C���Ԃ����Z
        if (m_elapsedWindowTime >= m_windowDelay) {
            m_isSlidingIn = true;  // �X���C�h�C���J�n
        }
        return; // ���̏������X�L�b�v
    }

    // �E�B���h�E�X���C�h�C������
    if (m_isSlidingIn) {
        m_slideProgress += deltaTime / m_slideDuration;
        if (m_slideProgress >= 1.0f) {
            m_slideProgress = 1.0f;
            m_isSlidingIn = false;
            m_windowVisible = true;
            m_isTyping = true;
        }
        return; // �X���C�h�C�����͑��̏������X�L�b�v
    }

    // �E�B���h�E�X���C�h�A�E�g����
    if (m_isSlidingOut) {
        m_slideProgress -= deltaTime / m_slideDuration; // �i�s�x������
        if (m_slideProgress <= 0.0f) { // ����
            m_slideProgress = 0.0f;
            m_isSlidingOut = false;
            m_windowVisible = false; // �E�B���h�E��\��
            m_isActive = false;      // �`���[�g���A���I��
        }
        return; // �X���C�h�A�E�g���͑��̏������X�L�b�v
    }

        // �^�C�s���O�G�t�F�N�g����
        if (m_charCount < m_steps[m_currentStep].size()) {
            m_elapsedTime += deltaTime;
            if (m_elapsedTime >= m_charDisplaySpeed) {
                m_charCount++;
                m_displayedText = m_steps[m_currentStep].substr(0, m_charCount);
                m_elapsedTime = 0.0f;

                // �^�C�s���O���̍Đ�
                SOUND("��")->Play(false);
            }
            m_isTyping = true;
        }
        else {
            m_isTyping = false;
            SOUND("��")->Stop();
        }

        // �����Ă���\��̐؂�ւ�
        if (m_isTyping) {
            m_faceTimer += deltaTime;
            if (m_faceTimer >= m_faceSwitchSpeed) {
                m_faceIndex = rand() % m_facePatterns.size();
                m_face = m_facePatterns[m_faceIndex];
                m_faceTimer = 0.0f;
            }
        }
        else {
            m_face = L":)"; // �^�C�s���O���I�������Ί�
        }


    // ��̐؂�ւ��A�j���[�V����
    if (m_isTyping) { // �^�C�s���O���̂݊�𓮂���
        m_faceTimer += deltaTime;
        if (m_faceTimer >= m_faceSwitchSpeed) {
            m_faceIndex = rand() % m_facePatterns.size();
            m_face = m_facePatterns[m_faceIndex];
            m_faceTimer = 0.0f; // �^�C�}�[�����Z�b�g
        }
    }
}

void TutorialManager::Render() {
    // �`����~��������𒲐�
    if (!m_isSlidingIn && !m_isSlidingOut && !m_windowVisible) return;

    Renderer renderer;

    int windowWidth = 800;
    int windowHeight = 150;
    int paddingX = 200;
    int paddingY = 300;

    // �X���C�h�C���E�A�E�g�̐i�s�x�ɉ�����X���W��ύX
    float startX = 1880;                     // ��ʉE�[
    float endX = 1280 - paddingX;            // �X���C�h��̍ŏI�ʒu
    int windowX = startX + (endX - startX) * m_slideProgress;
    int windowY = paddingY;

    // �e�L�X�g�E�B���h�E��`��
    renderer.DrawTextWindow(windowX, windowY, windowWidth, windowHeight, m_displayedText.c_str(),
        CVector4D(0.0f, 0.0f, 0.0f, 0.7f),
        CVector3D(1.0f, 1.0f, 1.0f),
        20);

    // ��A�C�R���̕`��
    int faceX = windowX + 20;
    int faceY = windowY + (windowHeight / 2) - 40;
    renderer.DrawText(m_face.c_str(), faceX, faceY, CVector3D(1.0f, 1.0f, 1.0f));
}

void TutorialManager::NextStep() {
    if (m_currentStep >= m_steps.size() - 1) {
        m_isSlidingOut = true; // �X���C�h�A�E�g�J�n
        return;
    }

    // ���݂̕��������ׂĕ\������Ă��Ȃ��ꍇ�͑S����\��
    if (m_charCount < m_steps[m_currentStep].size()) {
        m_charCount = m_steps[m_currentStep].size();
        m_displayedText = m_steps[m_currentStep];
        return;
    }

    // ���̃X�e�b�v�֐i��
    m_currentStep++;
    m_charCount = 0;          // �^�C�s���O�����Z�b�g
    m_displayedText = L"";    // �\������������Z�b�g
    m_elapsedTime = 0.0f;     // �o�ߎ��Ԃ����Z�b�g
    m_isTyping = true;
}

bool TutorialManager::IsFinished() const {
    return !m_isActive; // �A�N�e�B�u�łȂ��Ȃ犮�����Ă���Ƃ݂Ȃ�
}