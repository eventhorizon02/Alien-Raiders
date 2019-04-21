// the mystery mother ship that flies across the top of the screen every so often.

#ifndef __ship__
#define __ship__

#include <stdio.h>
#include <SDL.h>
#include "def.h"

enum motherDir { LEFT, RIGHT }; // the direction the mother ship flies.
typedef enum motherDir flyDir;

struct ship
{
    float m_posX;
    float m_posY;
    int m_velX;
    bool m_destroyQueue; // queue to remove;
    SDL_Rect m_collider;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_texture;
    void(*render)();
    void(*update)(float);
    void(*takeHit)(); // ship got hit
};

typedef struct ship Ship;

Ship* getNewShip(flyDir, SDL_Renderer*, SDL_Texture* );
void cleanUpShip(); // clean up the memories used by ship.

#endif /* __ship__ */
