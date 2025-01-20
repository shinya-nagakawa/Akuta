#pragma once
#include "../Base/Base.h"

class Renderer{
public:

    Renderer();
    ~Renderer();
    void Update();

    void Draw();

    void BlackShow(float duration);

    // 矩形を描画
    static void DrawRect(int x, int y, int width, int height, const CVector4D& color);

     void DrawTextWindow(int x, int y, int width, int height, const wchar_t* text,
        const CVector4D& bgColor = CVector4D(0.0f, 0.0f, 0.0f, 0.7f),
        const CVector3D& textColor = CVector3D(1.0f, 1.0f, 1.0f),
        int padding = 10);

    // テキスト描画
    static void DrawText(const wchar_t* text, int x, int y, const CVector3D& color); // ワイド文字対応

    // テクスチャ描画
    static void DrawTexture(CTexture* texture, int x, int y, int width = 0, int height = 0);

    // 9スライス描画（ウィンドウ背景用）
    static void DrawNineSlice(CTexture* texture, int x, int y, int width, int height, int sliceSize);

    // 初期化・終了処理
    static void Initialize();
    static void Shutdown();
    void DrawTestRect();
private:
    CImage m_img;

    float m_duration; //持続時間
    float m_timer;
};