 // data and functions associated with bullets.
#include <stdlib.h>
#include "bullet.h"

#define BULLET_UP_SPEED PLAY_AREA_HEIGHT
#define BULLET_DOWN_SPEED PLAY_AREA_HEIGHT * 0.5
#define ZIGZAG_DOWN_SPEED 300
#define GRND_Y_POS 720

static void update(float, Bullet*);
static void render(Bullet*);
static void explodeUdate(float,Bullet*);// the alternative update function to be run while explode image of bullet is displaying



//Bullet* newBullet = NULL;

Bullet* createNewBullet(SDL_Renderer* rend ,SDL_Texture* tex, float posX, float posY, bulletType type)
{
  Bullet* newBullet = malloc(sizeof(Bullet));
  newBullet->m_renderer = rend;
  newBullet->m_texture = tex;
  newBullet->update = &update;
  newBullet->render = &render;
  newBullet->m_posX = posX;
  newBullet->m_posY = posY;
  newBullet->m_collider.x = posX;
  newBullet->m_collider.y = posY;
  newBullet->m_collider.w = 6;
  newBullet->m_collider.h = 18;
  newBullet->m_destroyQueue = false;
  newBullet->m_nextBullet = false;
  newBullet->m_type = type;
  switch (type)
    {
        case DOWN: // enemy regular bullet.
        {
            newBullet->m_velocity = BULLET_DOWN_SPEED;
            newBullet->m_normalSpeed = BULLET_DOWN_SPEED;
            SDL_Rect r = { 56, 106, 6, 18 };
            newBullet->m_clip = r;
        }
            break;

        case ZIGZAG: // alien zigzag bullet
        {
            newBullet->m_velocity = ZIGZAG_DOWN_SPEED;
            newBullet->m_normalSpeed = ZIGZAG_DOWN_SPEED;
            SDL_Rect r = { 160, 128, 12, 28 };
            newBullet->m_collider.w = 12;
            newBullet->m_collider.h = 28;
            newBullet->m_clip = r;
            break;
        }

        default: // base bullet
        {
            newBullet->m_velocity = -BULLET_UP_SPEED;
            newBullet->m_normalSpeed = -BULLET_UP_SPEED;
            SDL_Rect r = {56, 106, 6, 18};
            newBullet->m_clip = r;
        }
            break;
    }

    SDL_Rect r = { 160, 160, 12, 28 }; // the area in the spritesheet where bullet exploding image sits.
    newBullet->m_explode_clip = r;

  return newBullet;
}

void update(float dt,Bullet* newBullet)
{
  newBullet->m_posY += newBullet->m_velocity * dt / 1000.0;
  if ( ((newBullet->m_posY) <= 0) || ((newBullet->m_posY) >=  GRND_Y_POS ))
  {
    newBullet->m_destroyQueue = true; // queue to destroy bullet, by the creator of the bullet.
    newBullet->m_nextBullet = true;  // bullet left screen, allow another shot.
  }
  // update the collider rect, just the x and y the width and height stay constant at 6 X 18
    newBullet->m_collider.x = newBullet->m_posX;
    newBullet->m_collider.y = newBullet->m_posY;
}

void render(Bullet* newBullet)
{

   const SDL_Rect renderQuad = { newBullet->m_posX, newBullet->m_posY, newBullet->m_clip.w, newBullet->m_clip.h };
   SDL_RenderCopy( newBullet->m_renderer,newBullet->m_texture, &newBullet->m_clip, &renderQuad );
}

// destroy the bullet, after leaving screen or hitting something.
void destroyBullet(Bullet* newBullet)
{
  free(newBullet);
  newBullet = NULL;
}

// bullet hit bullet.
void bulletExplode(Bullet* newBullet)
{
    newBullet->m_velocity = 0; // stop the bullet.
    newBullet->m_clip = newBullet->m_explode_clip; // change the image from live bullet to exploded bullet
    newBullet->m_posY -= 9; // it's going to be the base's bullet to show the explosition so it has to be move up half the way to where the middle of alien bullet is.
    newBullet->update = &explodeUdate;
}

// the alternative update function to be run while explode image of bullet is displaying.
static void explodeUdate(float dt, Bullet* newBullet)
{
    static float explodeTime = 0;
    explodeTime += dt;
    if (explodeTime >= 500)
    {
        explodeTime = 0;
        newBullet->m_destroyQueue = true;
        newBullet->m_nextBullet = true; // allow another bullet to be shot by base.
    }
}

// if bullet exits bunker and has little speed left, make it fly fast through remaining of screen
void bulletResumeNormalSpeed(Bullet* myBullet)
{
    myBullet->m_velocity = myBullet->m_normalSpeed;
}














