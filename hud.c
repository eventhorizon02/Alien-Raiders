
#include <SDL_ttf.h>
#include <string.h>
#include <stdlib.h>
#include "hud.h"
#include "def.h"
#include "utills.h"


static void render();
static void update(float);
static void cleanUp();
static void init();
static void gameOver();
static void renderOver(); // the game over render.
static void updateOver(float); // the game over update
static void renderReady(); // the render method to run during the 2 seconds 'ready' message.

static Hud* myHud = NULL;
TTF_Font *gFont = NULL;

Hud* createHud(SDL_Renderer* rend,SDL_Texture* tex)
{
    SDL_Color color = { 0, 250, 150, 255 };
    myHud = malloc(sizeof(Hud));
    myHud->render = &render;
    myHud->update = &update;
    myHud->cleanUp = &cleanUp;
    myHud->gameOver = &gameOver;
    myHud->m_renderer = rend;
    myHud->m_texture = tex;
    myHud->m_score = 0;
    myHud->m_lives = 3;
    myHud->m_hightScore = 0;
    myHud->m_high_score_tex = NULL;
    myHud->m_score_tex = NULL;
    init();
    myHud->m_game_over_tex = NULL;
    myHud->m_ready_tex = NULL;
    loadFromRenderedTexture(&myHud->m_game_over_tex, "GAME OVER", gFont, color ,rend ); // creates a texture from a string, stores it in the game over texture
    loadFromRenderedTexture(&myHud->m_ready_tex, "READY", gFont, color ,rend );
    return myHud;
}

static void update(float dt)
{
    SDL_Color color = { 0, 250, 150, 255 };
    // concacate "score" to the score strings.
    char* s1 = int_to_ascii(myHud->m_score);
    char* s2 = int_to_ascii(myHud->m_hightScore);
    size_t len = strlen(s1);
    char* str1 = malloc(len + 8); // 7 for the word score + 2 spaces + 1 spot for null terminator
    strcpy(str1, "SCORE  ");
    strcpy(str1 + 7, s1);
    free(s1);
    s1 = NULL;
    len = strlen(s2);
    char* str2 = malloc(len + 13);
    strcpy(str2, "HIGH SCORE  ");
    strcpy(str2 + 11, s2);
    free(s2);
    s2 = NULL;
    loadFromRenderedTexture(&myHud->m_score_tex, str1, gFont, color ,myHud->m_renderer ); // creates a texture from a string, stores it in the score texture.
    loadFromRenderedTexture(&myHud->m_high_score_tex, str2, gFont, color ,myHud->m_renderer);
    free(str1);
    free(str2);
    str1 = NULL;
    str2 = NULL;

    // update the hight score
    myHud->m_hightScore = max(myHud->m_score, myHud->m_hightScore);
}

static void updateOver(float dt)
{
    // do nothing.
}

static void render()
{
    //draw the ground
    SDL_Rect fillRect = { 0, 720, PLAY_AREA_WIDTH, 3 };
    SDL_SetRenderDrawColor( myHud->m_renderer, 0, 250, 150, 255 );
    SDL_RenderFillRect( myHud->m_renderer, &fillRect );

    // draw both score labels
    int width,height;
    SDL_QueryTexture(myHud->m_score_tex, NULL, NULL, &width, &height);
    SDL_Rect rect1 = {10,10,width,height};
    SDL_RenderCopy( myHud->m_renderer,myHud->m_score_tex, NULL,&rect1  );
    SDL_QueryTexture(myHud->m_high_score_tex, NULL, NULL, &width, &height);
    SDL_Rect rect2 = { 350,10,width,height};
    SDL_RenderCopy( myHud->m_renderer,myHud->m_high_score_tex, NULL, &rect2 );

    // draw the lives
    static SDL_Rect clip = { 224, 224, 32, 32 }; // the area in the sprite sheet where heat image sits.
    int livesX = PLAY_AREA_WIDTH;
    for ( int i = 1; i <= myHud->m_lives; ++i)
    {
       const SDL_Rect renderQuad = { livesX - i * 25, 10, 20, 20 };
       SDL_RenderCopy( myHud->m_renderer,myHud->m_texture, &clip, &renderQuad );
    }

}

// the game over render
static void renderOver()
{
    //draw the ground
    SDL_Rect fillRect = { 0, 720, PLAY_AREA_WIDTH, 3 };
    SDL_SetRenderDrawColor( myHud->m_renderer, 0, 250, 150, 255 );
    SDL_RenderFillRect( myHud->m_renderer, &fillRect );

    // draw both score labels
    int width,height;
    SDL_QueryTexture(myHud->m_score_tex, NULL, NULL, &width, &height);
    SDL_Rect rect1 = {10,10,width,height};
    SDL_RenderCopy( myHud->m_renderer,myHud->m_score_tex, NULL,&rect1  );
    SDL_QueryTexture(myHud->m_high_score_tex, NULL, NULL, &width, &height);
    SDL_Rect rect2 = { 350,10,width,height};
    SDL_RenderCopy( myHud->m_renderer,myHud->m_high_score_tex, NULL, &rect2 );
    //render 'game over'
    SDL_Rect rect = { 350,50,200,45 };
    SDL_RenderCopy( myHud->m_renderer,myHud->m_game_over_tex, NULL,&rect );
}

// the render method to run during the 2 seconds 'ready' message.
static void renderReady()
{
    //draw the ground
    SDL_Rect fillRect = { 0, 720, PLAY_AREA_WIDTH, 3 };
    SDL_SetRenderDrawColor( myHud->m_renderer, 0, 250, 150, 255 );
    SDL_RenderFillRect( myHud->m_renderer, &fillRect );

    // draw both score labels
    int width,height;
    SDL_QueryTexture(myHud->m_score_tex, NULL, NULL, &width, &height);
    SDL_Rect rect1 = {10,10,width,height};
    SDL_RenderCopy( myHud->m_renderer,myHud->m_score_tex, NULL,&rect1  );
    SDL_QueryTexture(myHud->m_high_score_tex, NULL, NULL, &width, &height);
    SDL_Rect rect2 = { 350,10,width,height};
    SDL_RenderCopy( myHud->m_renderer,myHud->m_high_score_tex, NULL, &rect2 );

    // draw the lives
    static SDL_Rect clip = { 224, 224, 32, 32 }; // the area in the sprite sheet where heat image sits.
    int livesX = PLAY_AREA_WIDTH;
    for ( int i = 1; i <= myHud->m_lives; ++i)
    {
        const SDL_Rect renderQuad = { livesX - i * 25, 10, 20, 20 };
        SDL_RenderCopy( myHud->m_renderer,myHud->m_texture, &clip, &renderQuad );
    }

    // render the 'ready' label
    SDL_Rect rect = { 350,50,200,45 };
    SDL_RenderCopy( myHud->m_renderer,myHud->m_ready_tex, NULL,&rect );
}

// free all the memory used by hud
static void cleanUp()
{
    freeTexture(myHud->m_texture);
    freeTexture(myHud->m_ready_tex);
    freeTexture(myHud->m_score_tex);
    freeTexture(myHud->m_high_score_tex);
    freeTexture(myHud->m_game_over_tex);
    free(gFont);
    gFont = NULL;
    free(myHud);
    myHud = NULL;
}


// init hud, load media.
static void init()
{
    //Open the font
    gFont = TTF_OpenFont( "pixelFont.ttf", 18 );
    if( gFont == NULL )
        printf( "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError() );
}


static void gameOver()
{
    myHud->render = &renderOver;
    myHud->update = &updateOver; // do nothing.
}

// change hud to display 'ready' message
void readyHud()
{
    myHud->update = &updateOver; // do nothing, recycle the one from game over.
    myHud->render = &renderReady;
}

// change out of the hud ready mode back to normal hud display.
void unreadyHud()
{
    myHud->update = &update;
    myHud->render = &render;
}























