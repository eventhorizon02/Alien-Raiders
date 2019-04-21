// heads up display, score, messages, etc.

#ifndef __hud__
#define __hud__

#include <stdio.h>
#include <SDL.h>
#include "def.h"

struct hud
{
    int m_score;
    int m_lives;
    int m_hightScore;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_texture;
    SDL_Texture* m_score_tex;
    SDL_Texture* m_high_score_tex;
    SDL_Texture* m_game_over_tex;
    SDL_Texture* m_ready_tex;
    void(*render)();
    void(*update)(float);
    void(*cleanUp)();
    void(*gameOver)();
};

typedef struct hud Hud;

Hud* createHud(SDL_Renderer*,SDL_Texture*);
void readyHud(); // change hud to display 'ready' message
void unreadyHud(); // change out of the hud ready mode back to normal hud display.

#endif /* __hud__ */
