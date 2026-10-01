#pragma once
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "boot.h"
#include "scene.h"
#include "node.h"

struct Glyph
{
    SDL_FRect textureSrc{};
    SDL_FRect transform{};
    SDL_FRect backgroundSrc{};
    SDL_FRect backgroundTransform{};
};

class Label : Node
{
public:
    Label(SDL_State& a) : state(a) {}


    enum LABEL_ALIGNMENT
    {
        LABEL_ALIGNMENT_STANDARD,
        LABEL_ALIGNMENT_CENTER,
    };

    enum LABEL_TYPE
    {
        LABEL_TYPE_NORMAL,
        LABEL_TYPE_BACKGROUND,
        LABEL_TYPE_SHADOW,
    };

    enum LABEL_COLOR
    {
        LABEL_COLOR_RED,
        LABEL_COLOR_GREEN,
        LABEL_COLOR_BLUE,
        LABEL_COLOR_YELLOW,
        LABEL_COLOR_WHITE,
        LABEL_COLOR_BLACK,
        LABEL_COLOR_MAGENTA,
        LABEL_COLOR_CYAN,
    };

    LABEL_TYPE labelType = LABEL_TYPE_NORMAL;
    LABEL_COLOR labelColor = LABEL_COLOR_WHITE;
    SDL_State& state;

    //SDL_Texture* texture;
    SDL_Texture* textureShadow;
    std::vector<Glyph> glyphs;
    std::vector<Glyph> glyphs_Shadow;
    std::vector<Glyph> glyphs_Background; // might use idk
    // proprties
    std::string text = "<unassigned>";



    int charPositionX = 0;
    int charPositionY = 0;
    int positionX = 0;
    int positionY = 0;
    int fontSize = 8;
    int fontColorR = 255;
    int fontColorG = 255;
    int fontColorB = 255;
    int fontColorA = 255;
    int fontBackgroundColorR = 0;
    int fontBackgroundColorG = 0;
    int fontBackgroundColorB = 0;
    int fontBackgroundColorA = 72;
    int fontShadowColorR = 0;
    int fontShadowColorG = 0;
    int fontShadowColorB = 0;
    int fontShadowColorA = 255;

    // BACKGROUND rect
    

    int charsSinceNewLine = 0;

    int shadowOffsetPositionX = 1;
    int shadowOffsetPositionY = 2;

    bool need_new_glyphs_to_be_generated = false;

	void Create();
	void SetText(std::string newText);
	void GenerateGlyphs();
	void SetFont();
	void SetColorRGB(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void SetColorEnum(LABEL_COLOR color);
	void SetShadowColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void Position(int x, int y);
	void Render();
	void Clear();
	void Destroy();
	void GenerateGlyphBasedOnChar(char input, int index);
};
