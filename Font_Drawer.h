/*==============================================================================

   ƒtƒHƒ“ƒg‚ð•`‰æ‚·‚é [Font_Drawer.h]
                                                         Author : Gu Anyi
                                                         Date   : 2026/07/01

--------------------------------------------------------------------------------

==============================================================================*/

#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <string>
#include <unordered_map>

struct FontChar
{
    int id = 0;

    DirectX::XMFLOAT2 position = { 0.0f,0.0f };
    float width = 0.0f;
    float height = 0.0f;

    float xOffset = 0.0f;
    float yOffset = 0.0f;
    float xAdvance = 0.0f;

    int page = 0;
    int chnl = 0;
};

class FontDrawer
{
public:

    bool LoadContent(const char* fntPath, ID3D11ShaderResourceView* texture);

    void DrawContent(const char* text, DirectX::XMFLOAT2 position, float scale = 1.0f, DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawContent(const wchar_t* text, DirectX::XMFLOAT2 position, float scale = 1.0f, DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f });
    
    //DirectX::XMFLOAT2 MeasureContent(const char* text, float scale = 1.0f) const;
    //DirectX::XMFLOAT2 MeasureContent(const wchar_t* text, float scale = 1.0f) const;

private:

    float GetValueFloat(const std::string& line, const std::string& key) const;
    int GetValueInt(const std::string& line, const std::string& key) const;
    void DrawCodePoint(int codePoint, float& penX, float& penY, float scale, DirectX::XMFLOAT4 color);

private:

    ID3D11ShaderResourceView* m_Texture = nullptr;

    std::unordered_map<int, FontChar> m_Chars;

    float m_TextureWidth = 1.0f;
    float m_TextureHeight = 1.0f;

    float m_LineHeight = 0.0f;

};
