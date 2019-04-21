// the mystery mother ship that flies across the top of the screen every so often.

#include <SDL_mixer.h>
#include <stdlib.h>
#include "ship.h"
#include "utills.h"

#define SHIP_Y_POS 50
#define SHIP_X_VELOCITY 200
#define SHIP_LEFT_START -50
#define SHIP_RIGHT_START PLAY_AREA_WIDTH + 50

static void init();
static void update(float);
static void render();
static void takeHit(); // mother ship took hit.
static void hitUpdate(float); // the update function after ship is hit.
static void hitRender(); // the render function after ship is hit.
static void loadSoundFX();

Ship* motherShip = NULL;
Mix_Chunk* gFlyBy = NULL;
Mix_Chunk* gShipHit = NULL;

Ship* getNewShip(flyDir dir, SDL_Renderer* rend, SDL_Texture* tex)
{
    motherShip = malloc(sizeof(Ship));
    motherShip->m_renderer = rend;
    motherShip->m_texture = tex;
    motherShip->m_velX = (dir == RIGHT) ? SHIP_X_VELOCITY : -1 * SHIP_X_VELOCITY;
    motherShip->m_posX = (dir == RIGHT) ? SHIP_LEFT_START : SHIP_RIGHT_START;
    motherShip->m_posY = SHIP_Y_POS;
    motherShip->update = &update;
    motherShip->render = &render;
    motherShip->takeHit= &takeHit;
    motherShip->m_destroyQueue = false;
    motherShip->m_collider.x = motherShip->m_posX;
    motherShip->m_collider.y = SHIP_Y_POS;
    motherShip->m_collider.w = 56;
    motherShip->m_collider.h = 28;
    init();
    return motherShip;
}

static void init()
{
    loadSoundFX();
    Mix_PlayChannel( -1, gFlyBy, -1 );
}

static void update(float dt)
{
    motherShip->m_posX += motherShip->m_velX * dt / 1000;
    if ( (motherShip->m_posX > SHIP_RIGHT_START) || (motherShip->m_posX < SHIP_LEFT_START)) // ship went of the edge of the screen, (hard < or >)
        motherShip->m_destroyQueue = true;
    // update the collider
    motherShip->m_collider.x = motherShip->m_posX; // the rest of the collider rect stays the same.
}

static void render()
{
    static SDL_Rect clip = { 64, 74, 56, 28 }; // the area in the sprite sheet where mother ship sits
    const SDL_Rect renderQuad = { motherShip->m_posX, SHIP_Y_POS, 56, 28 };
    SDL_RenderCopy( motherShip->m_renderer,motherShip->m_texture, &clip, &renderQuad );
}

// the update function after ship is hit
static void hitUpdate(float dt)
{
    static float hitTime = 0;
    hitTime += dt;
    if (hitTime >= 500)
    {
        motherShip->m_destroyQueue = true;
        hitTime = 0; // since hitTime is a static, reset it for the next mother ship to come
    }
}

// the render function after ship is hit
static void hitRender()
{
  static SDL_Rect clipShipHit = { 0, 228, 56, 28 }; // the area in the sprite sheet where mother ship explosion image sits
  const SDL_Rect renderQuad = { motherShip->m_posX, SHIP_Y_POS, 56, 28 };
  SDL_RenderCopy( motherShip->m_renderer,motherShip->m_texture, &clipShipHit, &renderQuad );
}

// clean up the memories used by ship
void cleanUpShip()
{
   if(gFlyBy) // could have freed by the take hit function, or not.
   Mix_FreeChunk( gFlyBy );
   Mix_FreeChunk( gShipHit );
   gFlyBy = NULL;
   gShipHit = NULL;
   free(motherShip);
   motherShip = NULL;
}

//mother ship took hit
static void takeHit()
{
    Mix_FreeChunk( gFlyBy );
    gFlyBy = NULL;
    Mix_PlayChannel( -1, gShipHit, 0 );
    motherShip->update = &hitUpdate;
    motherShip->render = &hitRender;
}

// load the sounds effects for the ship.
static void loadSoundFX()
{
    //Load sound effects
    gFlyBy = Mix_LoadWAV( "soundFX/flyby.wav" );
    if( gFlyBy == NULL )
        printf( "Failed to load flyby sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
    gShipHit = Mix_LoadWAV("soundFX/shipHit.wav");
    if ( gShipHit == NULL)
        printf( "Failed to load ship hit sound effect! SDL_mixer Error: %s\n",Mix_GetError() );
}













