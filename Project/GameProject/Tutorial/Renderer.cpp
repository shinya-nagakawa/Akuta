#include "Renderer.h"
#include "Game/Camera.h"

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
        m_timer -= 1.0f; // タイマーを減少
       
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

    // 射影行列を設定
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 1280, 720, 0, -1, 1); // 画面の左上が (0,0) の2D空間を作成

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // 背景色を描画
    glColor4f(color.x, color.y, color.z, color.w);
    glBegin(GL_QUADS);
    glVertex2f(x, y);                      // 左上
    glVertex2f(x + width, y);              // 右上
    glVertex2f(x + width, y + height);     // 右下
    glVertex2f(x, y + height);             // 左下
    glEnd();

    // 元の行列に戻す
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Renderer::DrawTextWindow(int x, int y, int width, int height, const wchar_t* text, const CVector4D& bgColor, const CVector3D& textColor, int padding)
{
    // === 1. 背景ウィンドウを描画 ===
    glDisable(GL_DEPTH_TEST); // 深度テストを無効化して最前面に描画
    m_img.SetPos(x, y);       // 背景ウィンドウの位置を設定
    m_img.SetSize(width, height); // 背景ウィンドウのサイズを設定
    m_img.SetColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w); // 背景色を設定
    m_img.Draw(); // 背景ウィンドウを描画
    glEnable(GL_DEPTH_TEST); // 深度テストを再び有効化

    // === 2. テキストを描画 ===
    CFont* font = FONT_T();
    if (font)
    {
        font->SetFontSize(32);
        //テキストの高さを考慮して中央ぞろえ
        int textWidth = font->GetTextWidth(text);
        int textHeight = font->GetTextHeight();

        int textX = x + (width - textWidth) / 2;
        int textY = y + (height + textHeight+30) / 2;

        font->Draw(textX, textY, textColor.x, textColor.y, textColor.z, text);

    }
}


void Renderer::DrawText(const wchar_t* text, int x, int y, const CVector3D& color, int fontSize) {
    CFont* font = FONT_T();
    if (!font) {
        // 必要に応じてフォントを初期化
        font = CFont::CreateInstance("DefaultFont", "path/to/default/font.ttf", fontSize);
    }

    // フォントサイズを設定
    font->SetFontSize(fontSize);

    // テキストを描画
    font->Draw(x, y, color.x, color.y, color.z, text);

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

    // 左上
    DrawTexture(texture, x, y, sliceSize, sliceSize);
    // 上部
    DrawTexture(texture, x + sliceSize, y, width - sliceSize * 2, sliceSize);
    // 右上
    DrawTexture(texture, x + width - sliceSize, y, sliceSize, sliceSize);

    // 左側
    DrawTexture(texture, x, y + sliceSize, sliceSize, height - sliceSize * 2);
    // 中央
    DrawTexture(texture, x + sliceSize, y + sliceSize, width - sliceSize * 2, height - sliceSize * 2);
    // 右側
    DrawTexture(texture, x + width - sliceSize, y + sliceSize, sliceSize, height - sliceSize * 2);

    // 左下
    DrawTexture(texture, x, y + height - sliceSize, sliceSize, sliceSize);
    // 下部
    DrawTexture(texture, x + sliceSize, y + height - sliceSize, width - sliceSize * 2, sliceSize);
    // 右下
    DrawTexture(texture, x + width - sliceSize, y + height - sliceSize, sliceSize, sliceSize);
}

void Renderer::Initialize() {
    // デフォルトフォントのロード
    CFont::CreateInstance("default", "path/to/default/font.ttf", 16);
}

void Renderer::Shutdown() {
    // リソース解放
    CFont::ClearInstance();
}

void Renderer::DrawTestRect()
{
    // `m_img` のプロパティを利用して描画
    m_img.SetPos(100, 500);           // 表示位置を設定
    m_img.SetSize(800, 150);          // 矩形のサイズを設定
    m_img.SetColor(0.0f, 0.0f, 0.0f, 0.7f); // 背景色（半透明の黒）を設定
    m_img.Draw();                     // 実際に描画
}

void Renderer::DrawWorldText(const CVector3D& worldPos, float r, float g, float b, const char* text)
{
    // カメラのインスタンスを取得
    Camera* camera = Camera::Instance();

    // カメラの行列を取得
    CMatrix viewMatrix = camera->GetViewMatrix();
    CMatrix projectionMatrix = camera->GetProjectionMatrix();

    // ワールド座標をスクリーン座標に変換
    CVector4D worldPos4(textWorldPos.x, textWorldPos.y, textWorldPos.z, 1.0f);
    CVector4D screenPos = projectionMatrix * viewMatrix * worldPos4;

    // スクリーン座標を正規化
    if (screenPos.w <= 0.0f) return; // 背面にある場合は表示しない
    screenPos.x /= screenPos.w;
    screenPos.y /= screenPos.w;
    screenPos.z /= screenPos.w;

    // スクリーン座標をピクセル単位に変換
    int screenX = static_cast<int>((screenPos.x * 0.5f + 0.5f) * SCREEN_WIDTH);
    int screenY = static_cast<int>((-screenPos.y * 0.5f + 0.5f) * SCREEN_HEIGHT);

    // テキストを描画
    FONT_T()->Draw(screenX, screenY, r, g, b, text);
}
