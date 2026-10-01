#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <vector>
#include <iostream>
#include <string>
#ifdef _DEBUG
    #include "debug_console.h"
#endif
#include "label.h"
#include "boot.h"

void Label::Create()
{
    std::cout << "[Create] Created a new label.\n";
}

void Label::SetText(std::string newText)
{
    text = newText;
    need_new_glyphs_to_be_generated = false;
    GenerateGlyphs();
    //std::cout << "[SetText] Set new text: '" << text << "'\n";
}

void Label::GenerateGlyphs()
{
    if (!need_new_glyphs_to_be_generated)
    {
        // create glyphs for every char.
        // here is the main check func. for loop. every char, check the char and create a glyph.
        for (int i = 0; i < text.size(); i++)
        {
            // with every character check the char
            GenerateGlyphBasedOnChar(text[i], charsSinceNewLine);
            //std::cout << "[GenerateGlyphs] Generating new glyph: '" << text[charsSinceNewLine] << "'\n";
            //std::cout << "[GenerateGlyphs] Characters since new line: '" << charsSinceNewLine << "'\n";
            if (text[i] != '\n')
            {
                charsSinceNewLine++;
                //std::cout << "[GenerateGlyphs] Glyph is not newline!\n";
            }
            else
            {
                charsSinceNewLine = 0;
                //std::cout << "[GenerateGlyphs] Glyph is newline!\n";
            }


        }
        // theoretically after the forloop is done, we should be done with generating glyphs.
        need_new_glyphs_to_be_generated = true;
        //std::cout << "[GenerateGlyphs] Finished generating glyphs.\n";
    }
}

void Label::SetFont()
{
    std::string texturePath = "textures/fonts/debug.png";
    texture = IMG_LoadTexture(state.renderer, texturePath.c_str());
    textureShadow = IMG_LoadTexture(state.renderer, texturePath.c_str());
    if (!texture)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "texture null reference", nullptr);
    }
    if (!textureShadow)
    {
        std::string errorString = "textureShadow null reference. path: " + texturePath + "";
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", errorString.c_str(), nullptr);
    }
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
}

void Label::SetColorRGB(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    for (int i = 0; i < glyphs.size(); i++)
    {
        SDL_SetTextureColorMod(texture, r, g, b);
        SDL_SetTextureAlphaMod(texture, a);
    }
}

void Label::SetColorEnum(LABEL_COLOR color)
{
    Uint8 r = 255;
    Uint8 g = 255;
    Uint8 b = 255;
    Uint8 a = 255;
    switch (color)
    {
        case LABEL_COLOR_RED:
            r = 255;
            g = 0;
            b = 0;
            a = 255;
            break;
        case LABEL_COLOR_GREEN:
            r = 0;
            g = 255;
            b = 0;
            a = 255;
            break;
        case LABEL_COLOR_BLUE:
            r = 0;
            g = 0;
            b = 255;
            a = 255;
            break;
        case LABEL_COLOR_YELLOW:
            r = 255;
            g = 255;
            b = 0;
            a = 255;
            break;
        case LABEL_COLOR_WHITE:
            r = 255;
            g = 255;
            b = 255;
            a = 255;
            break;
        case LABEL_COLOR_BLACK:
            r = 0;
            g = 0;
            b = 0;
            a = 255;
            break;
        case LABEL_COLOR_MAGENTA:
            r = 255;
            g = 0;
            b = 255;
            a = 255;
            break;
        case LABEL_COLOR_CYAN:
            r = 0;
            g = 255;
            b = 255;
            a = 255;
            break;
        default:
            break;
    }
    
    for (int i = 0; i < glyphs.size(); i++)
    {
        SDL_SetTextureColorMod(texture, r, g, b);
    }
}

void Label::SetShadowColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    if (labelType == LABEL_TYPE_SHADOW)
    {
        for (int i = 0; i < glyphs_Shadow.size(); i++)
        {
            SDL_SetTextureColorMod(textureShadow, r, g, b);
        }
    }
}

void Label::Render()
{
    // render shadow first
    if (labelType == LABEL_TYPE_SHADOW)
    {
        for (int i = 0; i < glyphs_Shadow.size(); i++)
        {
            SDL_RenderTexture(state.renderer, textureShadow, &glyphs_Shadow[i].textureSrc, &glyphs_Shadow[i].transform);
        }
    }

    // render the background first.
    if (labelType == LABEL_TYPE_BACKGROUND)
    {
        for (int i = 0; i < glyphs.size(); i++)
        {
            SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(state.renderer, fontBackgroundColorR, fontBackgroundColorG, fontBackgroundColorB, fontBackgroundColorA);
            SDL_RenderFillRect(state.renderer, &glyphs[i].backgroundTransform);
            if (!SDL_RenderFillRect(state.renderer, &glyphs[i].backgroundTransform))
            {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to create texture.", nullptr);
            }
        }
    }

    for (int i = 0; i < glyphs.size(); i++)
    {
        SDL_RenderTexture(state.renderer, texture, &glyphs[i].textureSrc, &glyphs[i].transform);
    }
}

void Label::Clear()
{
    text = "";
    charsSinceNewLine = 0;
    glyphs.clear();
    if (labelType == LABEL_TYPE_SHADOW)
    {
        glyphs_Shadow.clear();
    }
    charPositionY = 0;
}

void Label::Position(int x, int y)
{
    positionX = x;
    positionY = y;
}

void Label::Destroy()
{
    std::cout << "[Create] Destroyed label.\n";
}

// reallt badly done but who cares.
void Label::GenerateGlyphBasedOnChar(char input, int index)
{
    Glyph newGlyph;
    Glyph newGlyphShadow;

    // monospace size 8 so height is always 8.
    // theoreticaly this should work for any character in ascii from that range.
    if (input == 0x0A)
    {
        // newline with it starts at the beginning of the line.
        index = 0;
        // newline sets it a new line down.
        charPositionY += 8;

    }

    if (input > 0 && input < 255)
    {
        newGlyph.textureSrc.x = (input - 32) * 8;
        newGlyph.textureSrc.y = 0;
        newGlyph.textureSrc.w = 8;
        newGlyph.textureSrc.h = 8;

        newGlyph.transform.x = positionX + charPositionX + index * fontSize;
        newGlyph.transform.y = positionY + charPositionY;
        newGlyph.transform.w = fontSize;
        newGlyph.transform.h = fontSize;


        if (labelType == LABEL_TYPE_SHADOW)
        {
            newGlyphShadow.textureSrc.x = (input - 32) * 8;
            newGlyphShadow.textureSrc.y = 0;
            newGlyphShadow.textureSrc.w = 8;
            newGlyphShadow.textureSrc.h = 8;

            newGlyphShadow.transform.x = positionX + (charPositionX + index * fontSize) + shadowOffsetPositionX;
            newGlyphShadow.transform.y = positionY + charPositionY + shadowOffsetPositionY;
            newGlyphShadow.transform.w = fontSize;
            newGlyphShadow.transform.h = fontSize;
        }

        if (labelType == LABEL_TYPE_BACKGROUND)
        {
            //newGlyph.backgroundSrc.x = (input - 32) * 8;
            //newGlyph.backgroundSrc.y = 0;
            //newGlyph.backgroundSrc.w = 8;
            //newGlyph.backgroundSrc.h = 8;

            newGlyph.backgroundTransform.x = positionX + charPositionX + index * fontSize;
            newGlyph.backgroundTransform.y = positionY + charPositionY;
            newGlyph.backgroundTransform.w = fontSize;
            newGlyph.backgroundTransform.h = fontSize;
        }

    }
    // sepcial chars
    // newline

    // idk how dest. its the position of the texture so that gets calculated later down the pipeline

    glyphs.push_back(newGlyph);

    if (labelType == LABEL_TYPE_SHADOW)
    {
        glyphs_Shadow.push_back(newGlyphShadow);
    }
    //std::cout << "[GenerateGlyphBasedOnChar] Added glyph:\nX: " << newGlyph.textureSrc.x << "\nY: " << newGlyph.textureSrc.y << "\n";
}
