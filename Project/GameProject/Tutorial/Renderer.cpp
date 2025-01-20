#include "Renderer.h"

Renderer::Renderer() : m_duration(0.0f), m_timer(0.0)
{
    m_img = COPY_RESOURCE("BlackFilter", CImage);
}

Renderer::~Renderer()
{

}

void Renderer::Update()
{
    if (m_timer > 0.0f)
    {
        m_timer -= 1.0f; // �^�C�}�[������
       
    }
    else
    {
       
    }
}

void Renderer::Draw()
{
   
}

void Renderer::BlackShow(float duration)
{
    m_duration = duration;
    m_timer = duration;
}

void Renderer::DrawRect(int x, int y, int width, int height, const CVector4D& color) {

    // �ˉe�s���ݒ�
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 1280, 720, 0, -1, 1); // ��ʂ̍��オ (0,0) ��2D��Ԃ��쐬

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // �w�i�F��`��
    glColor4f(color.x, color.y, color.z, color.w);
    glBegin(GL_QUADS);
    glVertex2f(x, y);                      // ����
    glVertex2f(x + width, y);              // �E��
    glVertex2f(x + width, y + height);     // �E��
    glVertex2f(x, y + height);             // ����
    glEnd();

    // ���̍s��ɖ߂�
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Renderer::DrawTextWindow(int x, int y, int width, int height, const wchar_t* text, const CVector4D& bgColor, const CVector3D& textColor, int padding)
{
    // === 1. �w�i�E�B���h�E��`�� ===
    glDisable(GL_DEPTH_TEST); // �[�x�e�X�g�𖳌������čőO�ʂɕ`��
    m_img.SetPos(x, y);       // �w�i�E�B���h�E�̈ʒu��ݒ�
    m_img.SetSize(width, height); // �w�i�E�B���h�E�̃T�C�Y��ݒ�
    m_img.SetColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w); // �w�i�F��ݒ�
    m_img.Draw(); // �w�i�E�B���h�E��`��
    glEnable(GL_DEPTH_TEST); // �[�x�e�X�g���ĂїL����

    // === 2. �e�L�X�g��`�� ===
    CFont* font = FONT_T();
    if (font)
    {
        font->SetFontSize(32);
        //�e�L�X�g�̍������l�����Ē������낦
        int textWidth = font->GetTextWidth(text);
        int textHeight = font->GetTextHeight();

        int textX = x + (width - textWidth) / 2;
        int textY = y + (height + textHeight+30) / 2;

        font->Draw(textX, textY, textColor.x, textColor.y, textColor.z, text);

    }
}


void Renderer::DrawText(const wchar_t* text, int x, int y, const CVector3D& color)
{ 
    CFont* font = FONT_T();
    if (!font) {
        font = CFont::CreateInstance("DefaultFont", "path/to/default/font.ttf", 16); // �K�v�ɉ����ăt�H���g��������
    }

    font->SetFontSize(48); // �t�H���g�T�C�Y��ݒ�
    font->Draw(x, y, color.x, color.y, color.z, text); // wchar_t* ��`��
    font->SetFontSize(32);
}

void Renderer::DrawTexture(CTexture* texture, int x, int y, int width, int height) {
    if (!texture) return;

    if (width == 0) width = texture->GetWidth();
    if (height == 0) height = texture->GetHeight();

    texture->MapTexture();

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width, y);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x + width, y + height);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + height);
    glEnd();

    texture->UnmapTexture();
}

void Renderer::DrawNineSlice(CTexture* texture, int x, int y, int width, int height, int sliceSize) {
    if (!texture) return;

    // ����
    DrawTexture(texture, x, y, sliceSize, sliceSize);
    // �㕔
    DrawTexture(texture, x + sliceSize, y, width - sliceSize * 2, sliceSize);
    // �E��
    DrawTexture(texture, x + width - sliceSize, y, sliceSize, sliceSize);

    // ����
    DrawTexture(texture, x, y + sliceSize, sliceSize, height - sliceSize * 2);
    // ����
    DrawTexture(texture, x + sliceSize, y + sliceSize, width - sliceSize * 2, height - sliceSize * 2);
    // �E��
    DrawTexture(texture, x + width - sliceSize, y + sliceSize, sliceSize, height - sliceSize * 2);

    // ����
    DrawTexture(texture, x, y + height - sliceSize, sliceSize, sliceSize);
    // ����
    DrawTexture(texture, x + sliceSize, y + height - sliceSize, width - sliceSize * 2, sliceSize);
    // �E��
    DrawTexture(texture, x + width - sliceSize, y + height - sliceSize, sliceSize, sliceSize);
}

void Renderer::Initialize() {
    // �f�t�H���g�t�H���g�̃��[�h
    CFont::CreateInstance("default", "path/to/default/font.ttf", 16);
}

void Renderer::Shutdown() {
    // ���\�[�X���
    CFont::ClearInstance();
}

void Renderer::DrawTestRect()
{
    // `m_img` �̃v���p�e�B�𗘗p���ĕ`��
    m_img.SetPos(100, 500);           // �\���ʒu��ݒ�
    m_img.SetSize(800, 150);          // ��`�̃T�C�Y��ݒ�
    m_img.SetColor(0.0f, 0.0f, 0.0f, 0.7f); // �w�i�F�i�������̍��j��ݒ�
    m_img.Draw();                     // ���ۂɕ`��
}
