// all the data and functions associated with the base ship
#include <stdio.h>
#include <SDL_mixer.h>
#include <stdlib.h>
#include "base.h"
#include "utills.h"

#define PIXEL_PER_SEC PLAY_AREA_WIDTH / 5.0
#define BASE_Y_POS 687
#define BASE_LEFT_LIMIT 80
#define BASE_RIGHT_LIMIT PLAY_AREA_WIDTH - 136

static void processEvents(inputEvent );
static void update(float);
static void render();
static void fire(); // fire a shot, if allowed.
static void loadSoundFX(); // load the sounds effects for the ship.
static void init();
static void hitUpdate(float); // the other udate function to run during got hit animation
static void hitRender(); // same as above
static void hitProcessEvents(inputEvent ); // will just freeze input while hit
static void setClip( SDL_Rect* ); // needed to set the sprite clip for the got hit animation.
static void cleanUp(); // clean up sound fx loaded by base ship.


Base* homeBase = NULL;
Mix_Chunk* gFire = NULL;
Mix_Chunk* gDamage = NULL;
static SDL_Rect hitClip[3]; // the sprite clips for the hit animation

Base* getBase(SDL_Renderer* rend, SDL_Texture* tex)
{
   loadSoundFX();
   homeBase = malloc(sizeof(Base));
   homeBase->processEvents = &processEvents;
   homeBase->update = &update;
   homeBase->render = &render;
   homeBase->cleanUp= &cleanUp;
   homeBase->m_posX = BASE_LEFT_LIMIT;
   homeBase->m_renderer = rend;
   homeBase->m_texture = tex;
   homeBase->m_vel = 0.f;
   homeBase->m_timer = 0.f;
   homeBase->m_bullet = NULL;
   homeBase->m_canFire = true;
   homeBase->m_isHit = false;
   homeBase->m_isAmmune = true;
   homeBase->m_frame = 0;
   homeBase->m_collider.x = homeBase->m_posX;
   homeBase->m_collider.y = BASE_Y_POS;
   homeBase->m_collider.w = 56;
   homeBase->m_collider.h = 28;
   init();
   return homeBase;
}

static void init()
{
    // set the hit animation spritesheet clips
    hitClip[0].x = 0;
    hitClip[0].y = 138;
    hitClip[0].w = 56;
    hitClip[0].h = 28;
    hitClip[1].x = 0;
    hitClip[1].y = 110;
    hitClip[1].w = 56;
    hitClip[1].h = 28;
    hitClip[2].x = 32;
    hitClip[2].y = 46;
    hitClip[2].w = 56;
    hitClip[2].h = 28;
}

static void processEvents(inputEvent e)
{
   if ( isState(mvLeftSt) )
        homeBase->m_vel = PIXEL_PER_SEC * -1;
    else if ( isState(mvRightSt) )
        homeBase->m_vel = PIXEL_PER_SEC;
    else
        homeBase->m_vel = 0;

    if ( e == fireEv )
        fire();
}

static void update(float dt)
{
  homeBase->m_timer += dt;
  if (homeBase->m_timer >= 2000) homeBase->m_isAmmune = false;
  homeBase->m_posX += (homeBase->m_vel) * (dt / 1000); // dt is in milliseconds
  homeBase->m_posX = clamp(homeBase->m_posX , BASE_LEFT_LIMIT, BASE_RIGHT_LIMIT);

   if (homeBase->m_bullet)
   {
     if (homeBase->m_bullet->m_destroyQueue)
      {
         if (homeBase->m_bullet->m_nextBullet)
             homeBase->m_canFire = true;
          destroyBullet(homeBase->m_bullet);
         homeBase->m_bullet = NULL;
      }
     else
     (homeBase->m_bullet)->update(dt, homeBase->m_bullet);
   }
    // update the collider
    homeBase->m_collider.x = homeBase->m_posX; // the rest of the parameters of the collider rect stay the same.
}

// fire a shot if available
static void fire()
{
  if (!(homeBase->m_bullet) && (homeBase->m_canFire)) // if there isn't another bullet in existence already and firing is allowed
  {
    homeBase->m_bullet = createNewBullet(homeBase->m_renderer,homeBase->m_texture,homeBase->m_posX + 28, BASE_Y_POS, UP);
    Mix_PlayChannel( -1, gFire, 0 );
    homeBase->m_canFire = false;
  }
}

static void render()
{
   static SDL_Rect clip = { 96, 102, 56, 28 }; // the area in the sprite sheet where base sits.
   const SDL_Rect renderQuad = { homeBase->m_posX, BASE_Y_POS, 56, 28 };
   SDL_RenderCopy( homeBase->m_renderer,homeBase->m_texture, &clip, &renderQuad );

  if (homeBase->m_bullet)
   (homeBase->m_bullet)->render(homeBase->m_bullet);
}

 // load the sounds effects for the ship.
static void loadSoundFX()
{
  //Load sound effects
  gFire = Mix_LoadWAV( "soundFX/shot.wav" );
  if( gFire == NULL )
    printf( "Failed to load fire sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
  gDamage = Mix_LoadWAV("soundFX/damage.wav");
    if ( gDamage == NULL)
        printf( "Failed to load damage sound effect! SDL_mixer Error: %s\n",Mix_GetError() );

}

// clean up sound fx loaded by base ship and free memory.
static void cleanUp()
{
   Mix_FreeChunk( gFire );
   Mix_FreeChunk( gDamage );
}

// base took a bullet.
// change all the updat, render and process event functions after getting hit
void baseHit()
{
    if (homeBase->m_isHit) return; // no multiple hits on dead player
    homeBase->m_isHit = true;
    Mix_PlayChannel( -1, gDamage, 0 );
    homeBase->processEvents = &hitProcessEvents;
    homeBase->update = &hitUpdate;
    homeBase->render = &hitRender;
}

 // the other udate function to run during got hit animation
static void hitUpdate(float dt)
{
    static float totalTime = 0;
    totalTime += dt;

    if ( totalTime >= 100 )
    {
        totalTime = 0;
        homeBase->m_frame = ( (homeBase->m_frame) + 1) % 3;
    }
}

// the render function to run during the hit animation
static void hitRender()
{
    //Render current frame
    SDL_Rect* currentClip = &(hitClip[homeBase->m_frame]);
    setClip( currentClip);
    // and actuall rendering happens setClip
}

// will just freeze input while hit
static void hitProcessEvents( inputEvent e )
{
   // do nothing.
}

// set the clip from the spritesheet for the current animation frame of the got hit animation.
static void setClip( SDL_Rect* clip )
{
    int width,height; // the whole spritesheet width and height.
    SDL_QueryTexture( homeBase->m_texture, NULL, NULL, &width, &height);
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { homeBase->m_posX, BASE_Y_POS, width, height };
    //Set clip rendering dimensions
    if( clip != NULL )
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    //Render to screen
    SDL_RenderCopy( homeBase->m_renderer, homeBase->m_texture, clip, &renderQuad );
}


















