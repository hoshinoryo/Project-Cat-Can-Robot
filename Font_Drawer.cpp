/*==============================================================================

   ƒtƒHƒ“ƒg‚ð•`‰æ‚·‚é [Font_Drawer.cpp]
                                                         Author : Gu Anyi
                                                         Date   : 2026/07/01

--------------------------------------------------------------------------------

==============================================================================*/

#include "Font_Drawer.h"
#include "Sprite.h"

#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace DirectX;

bool FontDrawer::LoadContent(const char* fntPath, ID3D11ShaderResourceView* texture)
{
    m_Texture = texture;
    m_Chars.clear();

    std::ifstream file(fntPath);
    if (!file.is_open())
    {
        return false;
    }

    std::string line;

    while (std::getline(file, line))
    {
        if (line.find("common ") == 0)
        {
            m_LineHeight = GetValueFloat(line, "lineHeight");
            m_TextureWidth = GetValueFloat(line, "scaleW");
            m_TextureHeight = GetValueFloat(line, "scaleH");
        }
        else if (line.find("char id=") == 0)
        {
            FontChar ch{};

            ch.id = GetValueInt(line, "id");
            ch.position.x = GetValueFloat(line, "x");
            ch.position.y = GetValueFloat(line, "y");
            ch.width = GetValueFloat(line, "width");
            ch.height = GetValueFloat(line, "height");
            ch.xOffset = GetValueFloat(line, "xoffset");
            ch.yOffset = GetValueFloat(line, "yoffset");
            ch.xAdvance = GetValueFloat(line, "xadvance");
            ch.page = GetValueInt(line, "page");
            ch.chnl = GetValueInt(line, "chnl");

            m_Chars[ch.id] = ch;
        }
    }

    return true;
}

void FontDrawer::DrawContent(const char* text, DirectX::XMFLOAT2 position, float scale, DirectX::XMFLOAT4 color)
{
    if (!text || !m_Texture)
    {
        return;
    }

    float penX = position.x;
    float penY = position.y;

    for (const char* p = text; *p; ++p)
    {
        unsigned char c = static_cast<unsigned char>(*p);

        if (c == '\n')
        {
            penX = position.x;
            penY += m_LineHeight * scale;
            continue;
        }

        DrawCodePoint(c, penX, penY, scale, color);
    }
}

void FontDrawer::DrawContent(const wchar_t* text, DirectX::XMFLOAT2 position, float scale, DirectX::XMFLOAT4 color)
{
    if (!text || !m_Texture)
    {
        return;
    }

    float penX = position.x;
    float penY = position.y;

    for (const wchar_t* p = text; *p; ++p)
    {
        if (*p == L'\n')
        {
            penX = position.x;
            penY += m_LineHeight * scale;
            continue;
        }

        DrawCodePoint(static_cast<int>(*p), penX, penY, scale, color);
    }
}

float FontDrawer::GetValueFloat(const std::string& line, const std::string& key) const
{
    std::string searchKey = key + "=";
    size_t pos = line.find(searchKey);

    if (pos == std::string::npos)
    {
        return 0.0f;
    }

    pos += searchKey.length();

    size_t end = line.find(' ', pos);
    std::string value = line.substr(pos, end - pos);

    return static_cast<float>(std::atof(value.c_str()));
}

int FontDrawer::GetValueInt(const std::string& line, const std::string& key) const
{
    return static_cast<int>(GetValueFloat(line, key));
}

void FontDrawer::DrawCodePoint(int codePoint, float& penX, float& penY, float scale, DirectX::XMFLOAT4 color)
{
    auto it = m_Chars.find(codePoint);
    if (it == m_Chars.end())
    {
        return;
    }

    const FontChar& ch = it->second;

    XMFLOAT2 drawPos{};
    drawPos.x = penX + ch.xOffset * scale + ch.width * scale * 0.5f;
    drawPos.y = penY + ch.yOffset * scale + ch.height * scale * 0.5f;

    XMFLOAT2 drawSize{};
    drawSize.x = ch.width * scale;
    drawSize.y = ch.height * scale;

    XMFLOAT2 texPos{};
    texPos.x = ch.position.x / m_TextureWidth;
    texPos.y = ch.position.y / m_TextureHeight;

    XMFLOAT2 texScale{};
    texScale.x = ch.width / m_TextureWidth;
    texScale.y = ch.height / m_TextureHeight;

    DrawSpriteTexCoord(
        false,
        m_Texture,
        drawPos,
        drawSize,
        texPos,
        texScale,
        0.0f,
        color
    );

    penX += ch.xAdvance * scale;
}
