// data and functions associated with bullets
#ifndef __bullet__
#define __bullet__

#include <SDL.h>
#include "def.h"

typedef enum bullet_type { DOWN, UP, ZIGZAG } bulletType;

struct bullet
{
  float m_posX;
  float m_posY;
  float m_velocity; // a speed that could change if goes through a bunker
  float m_normalSpeed; // the default speed to the bullet.
  bool  m_nextBullet; // allow firing of another bullet
  bulletType m_type;
  SDL_Renderer* m_renderer;
  SDL_Texture* m_texture;
  SDL_Rect m_collider;
  SDL_Rect m_clip;
  SDL_Rect m_explode_clip; // the sprite clip for bullet hiting bullet image.
  bool m_destroyQueue;
  void(*render)(struct bullet*);
  void(*update)(float, struct bullet*);
};

typedef struct bullet Bullet;

Bullet* createNewBullet(SDL_Renderer*,SDL_Texture*,float,float, bulletType);
void destroyBullet(Bullet*); // destroy the bullet
void bulletExplode(Bullet*); // bullet hit bullet.
void bulletResumeNormalSpeed(Bullet*); // if bullet exits bunker and has little speed left, make it fly fast through remaining of screen

#endif

