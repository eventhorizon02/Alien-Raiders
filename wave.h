// the wave of aliens

#ifndef __wave__
#define __wave__

#include <stdio.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include "alien.h"
#include "def.h"
#include "base.h"
#include "bullet.h"

typedef enum moveDirection { kRight, kLeft, kDown } mvDir; // the direction to move the wave.

struct wave
{
    SDL_Renderer* m_renderer;
    SDL_Texture* m_texture;
    Alien* alArray[11][5];
    Base* m_base; // needed to enable firing again after alien explosion animation is over
    mvDir m_mode; // is the wave moving to the left or right.
    Bullet* m_bullet;
    int m_numAliens;
    float m_mysteryTime; // the mystery ship timer.
    float m_totalTime; // total accumulated time
    void(*update)(float);
    void(*render)();
};

typedef struct wave Wave;

Wave* getNewWave(int, SDL_Renderer*, SDL_Texture*, Base*);
void freeWave();
bool isAlienHit(SDL_Rect); // is any of the aliens in the wave hit by a bullet
bool isBaseHit(SDL_Rect); // is base hit by alien bullet
void waveFire(); // have wave fire a bullet from a random alien if no other alien bullet is on scree.
float getBottomEdge(); // get the y of the lowest alien
Alien* getButtonOfColoum(int); // get the buttom alien of a coulum (if it exists)


#endif /* wave_h */
