// the bariers protecting the base from the alien bullets
#include <SDL_image.h>
#include <stdlib.h>
#include <string.h>
#include "bunker.h"

#define BULLET_DECELLARATION 10
#define BULLET_THRESHOLD_SPEED 100 // the slowest a bullet should move

static void update(float, Bunker*);
static void render(Bunker*);
static void init(Bunker*);
static void getPixelXY(int, int* ,int*, Bunker*); //send over the pixel index number and populate with the X and y coordiantes of the pixel
static bool isInRect(int, int, SDL_Rect*); // are pixel coordinates inside a rect


Bunker* getNewBunker(SDL_Renderer* rend,SDL_Window* win ,float posX )
{
    Bunker* myBunker = malloc(sizeof(Bunker));
    myBunker->m_posX = posX;
    myBunker->m_renderer = rend;
    myBunker->m_window = win;
    myBunker->render = &render;
    myBunker->update = &update;
    init(myBunker);
    return myBunker;
}

static void update(float dt, Bunker* myBunker)
{

}

static void render(Bunker* myBunker)
{
    const SDL_Rect renderQuad = { myBunker->m_posX, BUNKER_Y_POS, BUNKER_WIDTH, BUNKER_HEIGHT };
    SDL_RenderCopy( myBunker->m_renderer, myBunker->m_newTexture, NULL, &renderQuad );
}

void init(Bunker* myBunker)
{
    SDL_Surface* loadedSurface = IMG_Load( "bunker.png" );
    //Convert surface to display format
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, SDL_GetWindowPixelFormat( myBunker->m_window ),0 );

    // load the sprite sheet and create a streaming type texture
    myBunker->m_newTexture = SDL_CreateTexture( myBunker->m_renderer, SDL_GetWindowPixelFormat( myBunker->m_window ), SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h );

    //Lock texture for manipulation
    SDL_LockTexture( myBunker->m_newTexture, NULL, &myBunker->m_pixels, &myBunker->m_pitch );

    //Copy loaded/formatted surface pixels
    memcpy( myBunker->m_pixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h );

    //Unlock texture to update
    SDL_UnlockTexture( myBunker->m_newTexture );
    myBunker->m_pixels = NULL;

    //Get rid of old formatted surface
    SDL_FreeSurface( formattedSurface );

    //Get rid of old loaded surface
    SDL_FreeSurface( loadedSurface );
}

// change the bunker texture and slow bulltes
void manipulate(Bunker* myBunker, Bullet* bull)
{
    //Lock texture for manipulation
    SDL_LockTexture( myBunker->m_newTexture, NULL, &myBunker->m_pixels, &myBunker->m_pitch );

    //Allocate format from window
    Uint32 format = SDL_GetWindowPixelFormat( myBunker->m_window );
    SDL_PixelFormat* mappingFormat = SDL_AllocFormat( format );

    //Get pixel data
    Uint32* pixels = (Uint32*)(myBunker->m_pixels);
    Uint32 black = SDL_MapRGB( mappingFormat, 0x00, 0x00, 0x00 );
    Uint8 r,g,b;
    //Color key pixels
    int startX = bull->m_collider.x - myBunker->m_posX;
    int endX = startX + bull->m_collider.w;
    int rowNum = 0;
    while (rowNum < BUNKER_HEIGHT)
    {
    for( int i = startX; i < endX; ++i ) // for better efficiency only iterate the pixels that could be in the bullets rect
     {
         int pixelNum = (rowNum * BUNKER_WIDTH) + i;
         SDL_GetRGB(pixels[pixelNum], mappingFormat, &r, &g, &b);
        int x,y;
        if( b )
        {
            getPixelXY(pixelNum, &x, &y, myBunker);
            if ( isInRect(x, y, &bull->m_collider) )
            {
              pixels[pixelNum] = black;
                // decellatate the bullet as it goes thorough the bunker
              if (bull->m_type == UP)
              {
                  bull->m_velocity += BULLET_DECELLARATION;
                  if (bull->m_velocity >= -BULLET_THRESHOLD_SPEED)
                  {
                      bull->m_destroyQueue = true;
                      bull->m_nextBullet = true;
                  }
              }else
              {
                  bull->m_velocity -= BULLET_DECELLARATION;
                  if (bull->m_velocity <= BULLET_THRESHOLD_SPEED)
                  {
                      bull->m_destroyQueue = true;
                      bull->m_nextBullet = true;
                  }
              }
            }
        }
     }
        rowNum++;
    }


    //Unlock texture to update
    SDL_UnlockTexture( myBunker->m_newTexture );
    myBunker->m_pixels = NULL;

    //Free format
    SDL_FreeFormat( mappingFormat );

}

// helper function, send over the pixel index number and populate with the X and y coordiantes of the pixel
static void getPixelXY(int index, int* x ,int* y, Bunker* myBunker)
{
    int rowNumber = index / BUNKER_WIDTH;
    *y = BUNKER_Y_POS + rowNumber;
    int columNumber = index - rowNumber  * BUNKER_WIDTH;
    *x = myBunker->m_posX + columNumber;
}

 // are pixel coordinates inside a rect
static bool isInRect(int x, int y, SDL_Rect* rect)
{
    if ( ( x < rect->x ) || ( x > rect->x + rect->w ) )
       return false;
    if ( ( y < rect->y ) || ( y > rect->y + rect->h) )
        return false;
    return true;
}

// change bunker as result of contact with alien.
void bunkerAlienContact(Bunker* myBunker, Alien* myAlien)
{
    //Lock texture for manipulation
    SDL_LockTexture( myBunker->m_newTexture, NULL, &myBunker->m_pixels, &myBunker->m_pitch );

    //Allocate format from window
    Uint32 format = SDL_GetWindowPixelFormat( myBunker->m_window );
    SDL_PixelFormat* mappingFormat = SDL_AllocFormat( format );

    //Get pixel data
    Uint32* pixels = (Uint32*)(myBunker->m_pixels);
    Uint32 black = SDL_MapRGB( mappingFormat, 0x00, 0x00, 0x00 );
    Uint8 r,g,b;
    //Color key pixels
    int startX = myAlien->m_collider.x - myBunker->m_posX;
    int endX = startX + myAlien->m_collider.w;
    int rowNum = 0;
    while (rowNum < BUNKER_HEIGHT)
    {
        for( int i = startX; i < endX; ++i ) // for better efficiency only iterate the pixels that could be in the Alien's rect
        {
            int pixelNum = (rowNum * BUNKER_WIDTH) + i;
            SDL_GetRGB(pixels[pixelNum], mappingFormat, &r, &g, &b);
            int x,y;
            if( b )
            {
                getPixelXY(pixelNum, &x, &y, myBunker);
                if ( isInRect(x, y, &myAlien->m_collider) )
                 pixels[pixelNum] = black;
            }
        }
        rowNum++;
    }

    //Unlock texture to update
    SDL_UnlockTexture( myBunker->m_newTexture );
    myBunker->m_pixels = NULL;

    //Free format
    SDL_FreeFormat( mappingFormat );

 }















