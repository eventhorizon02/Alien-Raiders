
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdlib.h>
#include "pauseScene.h"
#include "stateMachine.h"
#include "utills.h"

static void processEvents(inputEvent);
static void update(float);
static void render();
static void onEnter();
static void onExit();
static void populateRects(); // populate the star rects

Scene* pauseScene = NULL;
extern bool isRunning;

typedef struct pauseTextures
{
  SDL_Texture* t1;
  SDL_Texture* t2;
  SDL_Texture* t3;
  SDL_Texture* t4;
  SDL_Texture* t5;
  TTF_Font* f1;
  Mix_Chunk* gPause;
  SDL_Rect starRects[66];
  int starIndex; // the running star index.
}myTextures;

myTextures tex;

Scene* getPauseScene(SDL_Renderer* rend, SDL_Window* win)
{
    pauseScene = malloc(sizeof(Scene));
    pauseScene->m_renderer = rend;
    pauseScene->m_window = win;
    pauseScene->onEnter = &onEnter;
    pauseScene->onExit = &onExit;
    pauseScene->render = &render;
    pauseScene->processEvents = &processEvents;
    pauseScene->update = &update;
    pauseScene->m_next = NULL;

    //Load sound effect
    tex.gPause = Mix_LoadWAV( "soundFX/pause.wav" );
    tex.starIndex = 0;
    return pauseScene;
}

static void processEvents(inputEvent e)
{
   if ( e == pauseEv )
       pop();
    else if (e == quitEv)
        isRunning = false;

}
static void update(float dt)
{
    static float totalTime = 0;
    totalTime += dt;
    if ( totalTime > 200 )
    {
        tex.starIndex = ( tex.starIndex + 1) % 66;
        totalTime = 0;
    }

}
static void render()
{
    SDL_Rect fillRect = { 287, 235, 450, 300 };
    SDL_SetRenderDrawColor( pauseScene->m_renderer, 0, 0, 0, 255 );
    SDL_RenderFillRect( pauseScene->m_renderer, &fillRect );

    int width,height;
    // render first line of text
    SDL_QueryTexture(tex.t1, NULL, NULL, &width, &height);
    SDL_Rect rect1 = {512 - width / 2, 260, width, height};
    SDL_RenderCopy( pauseScene->m_renderer,tex.t1, NULL,&rect1 );

    // render second line of text
    SDL_QueryTexture(tex.t2, NULL, NULL, &width, &height);
    SDL_Rect rect2 = {512 - width / 2, 350, width, height};
    SDL_RenderCopy( pauseScene->m_renderer,tex.t2, NULL,&rect2);

    // render third line of text
    SDL_QueryTexture(tex.t3, NULL, NULL, &width, &height);
    SDL_Rect rect3 = {512 - width / 2, 410, width, height};
    SDL_RenderCopy( pauseScene->m_renderer,tex.t3, NULL,&rect3 );

    // draw the stars
    for ( int i = 0; i < 66; ++i)
    {
        SDL_Rect star = tex.starRects[i];
        SDL_RenderCopy ( pauseScene->m_renderer, tex.t4, NULL, &star);
    }
    // draw the white star
    SDL_Rect white = tex.starRects[tex.starIndex];
    SDL_RenderCopy ( pauseScene->m_renderer, tex.t5, NULL, &white);
}
static void onEnter()
{
    Mix_PlayChannel( -1, tex.gPause, 0 );
    //Open the fonts
    tex.f1 = TTF_OpenFont( "pixelFont.ttf", 28 );
    SDL_Color color = { 0, 250, 150, 255 };
    SDL_Color white = { 255, 255, 255, 255 }; // the white running star
    loadFromRenderedTexture(&tex.t1, "PAUSED",tex.f1, color ,pauseScene->m_renderer ); // creates a texture from a string, stores
    loadFromRenderedTexture(&tex.t2, "ESC - TO CONTINUE",tex.f1, color ,pauseScene->m_renderer );
    loadFromRenderedTexture(&tex.t3, "Q - TO QUIT",tex.f1, color ,pauseScene->m_renderer );
    loadFromRenderedTexture(&tex.t4, "*",tex.f1, color ,pauseScene->m_renderer );
    loadFromRenderedTexture(&tex.t5, "*",tex.f1, white ,pauseScene->m_renderer ); // the white running star.
    // load the stars coordinates
    populateRects();
}
static void onExit()
{
    Mix_PlayChannel( -1, tex.gPause, 0 );
    freeTexture(tex.t1);
    freeTexture(tex.t2);
    freeTexture(tex.t3);
    freeTexture(tex.t4);
    freeTexture(tex.t5);
}

 // populate the star rects
static void populateRects()
{
    // they need to be sequenced in clockwise order
    // add the top and bottom lines
    for ( int i = 0; i < 18; ++i)
    {
        SDL_Rect rectTop = { 287 + i * 25, 227, 22, 28 };
        tex.starRects[i] = rectTop;
        SDL_Rect rectBottom = { 712 - i * 25, 515, 22, 28 };
        tex.starRects[33 + i] = rectBottom;
    }

    for ( int i = 1; i < 16; ++i)
    {
        SDL_Rect rectRight = { 712, 227 + i * 18, 22, 28 };
        tex.starRects[ 17 + i] = rectRight;

        SDL_Rect rectLeft = { 287, 515 - i * 18, 22, 28 };
        tex.starRects[ 50 + i] = rectLeft;
    }
}






















