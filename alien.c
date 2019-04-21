// data and functions associated with the invader ships
#include <stdio.h>
#include <stdlib.h>
#include <SDL_mixer.h>
#include "alien.h"

static void init(unsigned int, Alien*);
static void update(float,Alien*);
static void hitUpdate(float, Alien*); // the update function to be run after alien is hit.
static void render(Alien*);
static Bullet* fire(); // alien drop bomb on player.
void setClip( int x, int y, SDL_Rect* clip,Alien* ); //set the sprite sheet clip

Mix_Chunk* gHit = NULL;


Alien* getNewAlien(SDL_Renderer* rend,SDL_Texture* tex,float posX, float posY, unsigned int alienNumber)
{
  Alien* myAlien = malloc(sizeof(Alien));
  init(alienNumber,myAlien);
  myAlien->update = &update;
  myAlien->render = &render;
  myAlien->fire = &fire;
  myAlien->m_posX = posX;
  myAlien->m_posY = posY;
  myAlien->m_totalTime = 0.0;
  myAlien->frame = 0;
  myAlien->m_destroyQueue = false;
  myAlien->m_isHit = false;
  myAlien->m_renderer = rend;
  myAlien->m_texture = tex;
  myAlien->m_collider.x = posX;
  myAlien->m_collider.y = posY;
  myAlien->m_collider.w = 32;
  myAlien->m_collider.h = 32;
  SDL_QueryTexture(myAlien->m_texture, NULL, NULL, &myAlien->m_width, &myAlien->m_height);
  return myAlien;
}

// init animation clips and sound.
static void init(unsigned alienNumber,Alien* myAlien)
{
    myAlien->m_spriteClips = malloc( 2 * sizeof(SDL_Rect));
   // there are 8 different types of aliens, each has 2 frames, each has 2 coordinates.
   const static short coord[32] = {128,130,88,32,0,46,64,102,32,74,152,64,128,0,152,96,0,78,120,32,64,0,96,0,56,134,96,130,152,32,160,0};
   unsigned int start_index = alienNumber * 4;
   //Set sprite clips
   myAlien->m_spriteClips[ 0 ].x = coord[start_index];
   myAlien->m_spriteClips[ 0 ].y = coord[start_index + 1];
   myAlien->m_spriteClips[ 0 ].w = 32;
   myAlien->m_spriteClips[ 0 ].h = 32;

   myAlien->m_spriteClips[ 1 ].x = coord[start_index + 2];
   myAlien->m_spriteClips[ 1 ].y = coord[start_index + 3];
   myAlien->m_spriteClips[ 1 ].w = 32;
   myAlien->m_spriteClips[ 1 ].h = 32;

   myAlien->m_hitClip.x = 120;
   myAlien->m_hitClip.y = 64;
   myAlien->m_hitClip.w = 32;
   myAlien->m_hitClip.h = 32;

    //Load sound effect
    gHit = Mix_LoadWAV( "soundFX/hit.wav" );
    if( gHit == NULL )
        printf( "Failed to load hit sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
}

static void update(float dt,Alien* myAlien)
{
  // update the collider rect x and y position, the width and height stay constant at 32 X 32
    myAlien->m_collider.x = myAlien->m_posX;
    myAlien->m_collider.y = myAlien->m_posY;
}

// the update function to be run after alien is hit.
static void hitUpdate(float dt, Alien* myAlien)
{
   myAlien->m_totalTime += dt;
    if ( myAlien->m_totalTime >= 500 )
    {
        myAlien->m_totalTime = 0;
        if(myAlien->m_isHit) myAlien->m_destroyQueue = true;
    }
}

static void render(Alien* myAlien)
{
  //Render current frame
   SDL_Rect* currentClip = &(myAlien->m_spriteClips[myAlien->frame]);
   setClip( myAlien->m_posX, myAlien->m_posY, currentClip,myAlien );
}

// alien drop bomb on player
static Bullet* fire(Alien* myAlien)
{
    int rnd = rand() % 2; // a random number either 0 or 1.
    bulletType type =  ( rnd == 0 ) ? ZIGZAG : DOWN;
    return  createNewBullet(myAlien->m_renderer, myAlien->m_texture, myAlien->m_posX + 15, myAlien->m_posY + 32, type);
}

void setClip( int x, int y, SDL_Rect* clip,Alien* myAlien )
{
   //Set rendering space and render to screen
   SDL_Rect renderQuad = { x, y, myAlien->m_width, myAlien->m_height };
   //Set clip rendering dimensions
    if( clip != NULL )
    {
     renderQuad.w = clip->w;
     renderQuad.h = clip->h;
    }
   //Render to screen
   SDL_RenderCopy( myAlien->m_renderer, myAlien->m_texture, clip, &renderQuad );
}

// alien got hit
void alienHit(Alien* myAlien)
{
    if (myAlien->m_isHit) return; // avoid muliple hits on dead alien.
    Mix_PlayChannel( -1, gHit, 0 );
    myAlien->m_isHit = true;
    myAlien->m_totalTime = 0.0;
    myAlien->update = &hitUpdate;
    // make both animation clip rects the same one of the dectruction clip (the m_hitClip)
    myAlien->m_spriteClips[0] = myAlien->m_hitClip; // simple structure assignmet should work fine (no pointers in SDL_rect)
    myAlien->m_spriteClips[1] = myAlien->m_hitClip;
}

// free the memory used by this alien
void freeAlien(Alien* myAlien)
{
    free( myAlien->m_spriteClips );
    myAlien->m_spriteClips = NULL;
    free(myAlien);
    myAlien = NULL;
}

// change the animation frame
void alienFrameChage(Alien* myAlien)
{
   (myAlien->frame) = ( (myAlien->frame) + 1) % 2;
}




















