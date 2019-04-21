
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "introScene.h"
#include "inputCtrl.h"
#include "stateMachine.h"
#include "playScene.h"
#include "utills.h"
#include "alien.h"

static void processEvents(inputEvent);
static void update(float);
static void render();
static void onEnter();
static void onExit();
static bool loadMedia();
static void readScoreFile();
static void buildNameTex(); // create the high score name textures

Scene* introScene = NULL;
extern bool isRunning;

typedef struct textures
{
    SDL_Texture* t1;
    SDL_Texture* t2;
    SDL_Texture* t3;
    SDL_Texture* t4;
    SDL_Texture* t5;
    SDL_Texture* t6;
    SDL_Texture* t7;
    SDL_Texture* name_tex[5]; // the high scores name textures
    TTF_Font* f1;
    TTF_Font* f2;
    Alien* actor;
    Mix_Chunk* gStart;
    char* names[5]; // holds the names of the hi scorers
    int scores[5]; // holds the corresponding socres

}Textures;

Textures myTextures;

Scene* getIntroScene(SDL_Renderer* rend, SDL_Window* win)
{
    introScene = malloc(sizeof(Scene));
    introScene->m_renderer = rend;
    introScene->m_window = win;
    introScene->onEnter = &onEnter;
    introScene->onExit = &onExit;
    introScene->update = &update;
    introScene->render = &render;
    introScene->processEvents = &processEvents;
    introScene->m_next = NULL;
    introScene->m_texture = NULL;

    //Load sound effect
    myTextures.gStart = Mix_LoadWAV( "soundFX/start.wav" );

    return introScene;
}

static void onEnter()
{
    //Open the fonts
    myTextures.f1 = TTF_OpenFont( "pixelFont.ttf", 28 );
    myTextures.f2 = TTF_OpenFont( "pixelFont.ttf", 68 );
    SDL_Color color = { 0, 250, 150, 255 };
    SDL_Color color2 = { 255, 255, 0, 255 };


    loadFromRenderedTexture(&myTextures.t1, "INSERT COIN",myTextures.f1, color ,introScene->m_renderer ); // creates a texture from a string, stores
    loadFromRenderedTexture(&myTextures.t2, "ALIEN",myTextures.f2, color2 ,introScene->m_renderer );
    loadFromRenderedTexture(&myTextures.t3, "RAIDERS",myTextures.f2, color2 ,introScene->m_renderer );
    loadFromRenderedTexture(&myTextures.t4, "BENNY ABRAMOVICI",myTextures.f1, color ,introScene->m_renderer );
    loadFromRenderedTexture(&myTextures.t5, "PRESENTS:",myTextures.f1, color ,introScene->m_renderer );
    loadFromRenderedTexture(&myTextures.t7, "HI SCORES:",myTextures.f1, color ,introScene->m_renderer );
    loadMedia();

    // get one alien
    myTextures.actor = getNewAlien(introScene->m_renderer, introScene->m_texture, 480, 350, 2);

    // read the high scores files.
    readScoreFile();

    // build the name textures
    buildNameTex();
}

static void onExit()
{
    free(myTextures.f1);
    free(myTextures.f2);
    freeTexture(myTextures.t1);
    freeTexture(myTextures.t2);
    freeTexture(myTextures.t3);
    freeTexture(myTextures.t4);
    freeTexture(myTextures.t5);
    freeTexture(myTextures.t6);
    freeTexture(myTextures.t7);

    myTextures.f1 = NULL;
    myTextures.f2 = NULL;

    for ( int i = 0; i < 5; ++i)
    {
        freeTexture(myTextures.name_tex[i]);
        free(myTextures.names[i]);
        myTextures.names[i] = NULL;
        // the scores array has been freed already.
    }

    freeTexture(introScene->m_texture);
}

static void render()
{

    int width,height;

    SDL_QueryTexture(myTextures.t2, NULL, NULL, &width, &height);
    SDL_Rect rect2 = {512 - width / 2, 150, width, height};
    SDL_RenderCopy( introScene->m_renderer,myTextures.t2, NULL,&rect2 );

    SDL_QueryTexture(myTextures.t3, NULL, NULL, &width, &height);
    SDL_Rect rect3 = {512 - width / 2, 250, width, height};
    SDL_RenderCopy( introScene->m_renderer,myTextures.t3, NULL,&rect3 );

    SDL_QueryTexture(myTextures.t4, NULL, NULL, &width, &height);
    SDL_Rect rect4 = {512 - width / 2, 50, width, height};
    SDL_RenderCopy( introScene->m_renderer,myTextures.t4, NULL,&rect4 );

    SDL_QueryTexture(myTextures.t5, NULL, NULL, &width, &height);
    SDL_Rect rect5 = {512 - width / 2, 100, width, height};
    SDL_RenderCopy( introScene->m_renderer,myTextures.t5, NULL,&rect5);

    // render the "insert coin"
    SDL_QueryTexture(myTextures.t1, NULL, NULL, &width, &height);
    SDL_Rect rect = {512 - width / 2, 400, width, height};
    SDL_RenderCopy( introScene->m_renderer,myTextures.t6, NULL,&rect );

    myTextures.actor->render(myTextures.actor);

    // render 'hi scores:'
    SDL_QueryTexture(myTextures.t7, NULL, NULL, &width, &height);
    SDL_Rect rect7 = {512 - width / 2, 500, width, height};
    SDL_RenderCopy( introScene->m_renderer,myTextures.t7, NULL,&rect7 );

    // render the hight score names
    for ( int i = 0; i < 5; ++i )
    {
        SDL_QueryTexture(myTextures.name_tex[i], NULL, NULL, &width, &height);
        SDL_Rect rect = {512 - width / 2, 550 + i * 30, width, height};
        SDL_RenderCopy( introScene->m_renderer,myTextures.name_tex[i], NULL,&rect );
    }

}

static void processEvents(inputEvent e)
{
    if ( e == fireEv)
    {
        Mix_PlayChannel( -1, myTextures.gStart, 0 ); // play a sound effect
        Scene* playScene = getPlayScene(introScene->m_renderer, introScene->m_window);
        changeState(playScene);
    }
    else if (e == quitEv)
      isRunning = false;
}

static void update(float dt)
{
    static int n = 0;
    static float total = 0;
    total += dt;
    if ( total > 500)
    {
        total = 0;
        n = ( n + 1) % 2;
    }
    myTextures.t6 = (n) ? myTextures.t1 : NULL;
}

static bool loadMedia()
{
    //Loading success flag
    bool success = true;
    //Load sprite sheet texture

    // get rid of pre-existing texture
    freeTexture(introScene->m_texture);
    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load("invaders.png");
    if( !loadedSurface )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", "invaders.png", IMG_GetError() );
        success = false;
    }

    //Color key image
    SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

    //create texture from surface pixels
    introScene->m_texture = SDL_CreateTextureFromSurface( introScene->m_renderer, loadedSurface );
    if( !(introScene->m_texture) )
    {
        printf( "Unable to create texture from %s! SDL Error: %s\n", "invaders.png", SDL_GetError() );
        success = false;
    }

    //Get rid of old loaded surface
    SDL_FreeSurface( loadedSurface );

    return success;
}

static void readScoreFile()
{
    char* buf = file_read("highScores.txt");
    myGets(NULL, NULL,true); // reset the fseek to 0;
    int odds = 0;
    int evens = 0;
    for ( int lineNumber = 0; lineNumber < 10; ++lineNumber)
    {
       if ( (lineNumber % 2) == 0)
       {
           char* name = malloc(12);
           myGets(buf, name,false);
           unsigned long len = strlen(name);
           *(name + len - 1) = '\0'; // get rid of the new line char
           myTextures.names[evens] = name;
           ++evens;
       }else
       {
           char* num = malloc(8);
           myGets(buf, num,false);
           myTextures.scores[odds] = atoi(num);
           ++odds;
           free(num);
           num = NULL;
       }
    }

    free(buf);
    buf = NULL;
}

// create the high score name textures
static void buildNameTex()
{
      SDL_Color color = { 255, 255, 255, 255 }; // white color
      char* label = malloc(21); // 21 for max player name is 10 plus 6 for max score of 500,000 points plus spaces and sequence.

    for ( int i = 0; i < 5; ++i )
    {
        for ( int j = 0; j < 21; ++j )
            *(label + j ) = ' ';

        *(label + 20) = '\0';
        *(label) = i + 1 + '0';
        *(label + 1) = ':';
        // add the name to the label
        char* name = myTextures.names[i];
        unsigned long len = strlen(myTextures.names[i]);
        for ( int counter = 0; counter < len; ++counter)
        {
            *(label + counter + 2) = *name;
            ++name;
        }
        // add the score to the label
        char* str = int_to_ascii(myTextures.scores[i]);
        len = strlen(str);
        for ( int counter = 0; counter < len; ++ counter )
        *(label + counter + 13) = *(str + counter); // 13 is for a max name length of 10 plus sequence number plus ':' plus a space
        free(str);// the int_to_ascii alloces memory that needs to be freed.
        str = NULL;
        loadFromRenderedTexture(&myTextures.name_tex[i], label,myTextures.f1, color ,introScene->m_renderer );
    }

    free(label);
    label = NULL;
}



















