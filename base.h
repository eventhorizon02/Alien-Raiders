// all the data and functions associated with the base ship.

#ifndef __base__
#define __base__

#include <SDL.h>
#include "bullet.h"
#include "def.h"
#include "inputCtrl.h"

struct base
{
  float m_posX;
  float m_vel;
  float m_timer;
  Bullet* m_bullet;
  bool m_canFire;
  bool m_isHit;
  bool m_isAmmune; // don't fire at player for the first two seconds after it restarts
  int m_frame; // the current frame in the hit animation.
  SDL_Rect m_collider;
  SDL_Renderer* m_renderer;
  SDL_Texture* m_texture;
  void(*processEvents)(inputEvent);
  void(*render)();
  void(*update)(float);
  void(*cleanUp)();
};

typedef struct base Base;

Base* getBase(SDL_Renderer*,SDL_Texture*);
void baseHit(); // base took a bullet.

#endif



