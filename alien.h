// data and functions associated with the invader ships
#ifndef __alien__
#define __alien__
#include <SDL.h>
#include "def.h"
#include "bullet.h"


struct alien
{
  float m_posX;
  float m_posY;
  SDL_Rect m_collider;
  SDL_Renderer* m_renderer;
  SDL_Texture* m_texture;
  SDL_Rect* m_spriteClips;
  SDL_Rect m_hitClip; // alien got hit texture.
  float m_totalTime; // accumulated frame times.
  int m_width; // the whole spritesheet width.
  int m_height;
  int frame; // the current frame in the alien's animation
  bool m_destroyQueue; // queue to remove;
  bool m_isHit; // alien is hit
  void(*render)(struct alien*);
  void(*update)(float,struct alien*);
  Bullet*(*fire)(struct alien*);
};

typedef struct alien Alien;

Alien* getNewAlien(SDL_Renderer*,SDL_Texture*, float, float, unsigned int);
void alienHit(Alien*);
void freeAlien(Alien*); // free the memory used by this alien.
void alienFrameChage(Alien*); // change the animation frame

#endif



