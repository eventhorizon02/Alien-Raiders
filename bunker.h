// the bariers protecting the base from the alien bullets

#ifndef __bunker__
#define __bunker__

#define BUNKER_Y_POS 630
#define BUNKER_WIDTH 64
#define BUNKER_HEIGHT 46

#include <stdio.h>
#include <SDL.h>
#include "bullet.h"
#include "alien.h"

struct buker
{
    float m_posX;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_newTexture; // the texture to be modified
    SDL_Window* m_window;
    void* m_pixels;
    int m_pitch;
    void(*update)(float, struct buker*);
    void(*render)(struct buker*);
};

typedef struct buker Bunker;

Bunker* getNewBunker(SDL_Renderer*, SDL_Window* ,float );
void manipulate(Bunker*, Bullet*); // change the bunker texture and slow bulltes
void bunkerAlienContact(Bunker*, Alien*); // change bunker as result of contact with alien.

#endif /* __bunker__ */
