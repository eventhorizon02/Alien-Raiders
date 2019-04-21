
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdlib.h>

#include "victoryScene.h"
#include "scoreScene.h"
#include "introScene.h"
#include "stateMachine.h"
#include "utills.h"

static void processEvents(inputEvent);
static void update1(float); // first stage update function
static void update2(float); // second....
static void update3(float);
static void render1(); // first stage render function
static void render2();
static void render3();
static void onEnter();
static void onExit();
static void readScoreFile();
static bool loadMedia();
static void createFireWork(int); // create a new fire work at index.

Scene* victoryScene = NULL;
extern bool isRunning;

typedef  struct vars
{
    SDL_Texture* t1;
    SDL_Texture* t2;
    SDL_Texture* t3; // a blinking texture
    TTF_Font* f1;
    int lowest_score; // the lowest score found int he socre file
    float total_time;
    Mix_Chunk* gSound;
    Mix_Chunk* gPop;
    FILE *fp;
    SDL_Rect fireWorks[10]; // will holds the rects of the fire works
    float fireWorksTimer;
}Vars;

static Vars myVars;

Scene* getVictoryScene(SDL_Renderer* rend , SDL_Window* win)
{
    victoryScene = malloc(sizeof(Scene));
    victoryScene->m_renderer = rend;
    victoryScene->m_window = win;
    victoryScene->onEnter = &onEnter;
    victoryScene->onExit = &onExit;
    victoryScene->update = &update1;
    victoryScene->render = &render1;
    victoryScene->processEvents = &processEvents;
    victoryScene->m_next = NULL;
    victoryScene->m_texture = NULL;

    //Load sound effects
    myVars.gSound = Mix_LoadWAV( "soundFX/tadah.wav" );
    myVars.gPop = Mix_LoadWAV( "soundFX/hit.wav" );

    myVars.t3 = NULL;
    myVars.fireWorksTimer = 0;

    return victoryScene;
}

static void processEvents(inputEvent e)
{



    // if there was a score of less than 500,000 go to record name scene, else go back to intro
    if (e == fireEv)
    {
       if (myVars.lowest_score < 500000)
       {
           Scene* scoreScene = getScoreScene(victoryScene->m_renderer, victoryScene->m_window, 500000); // 500k is the max score, that why we are here
           changeState(scoreScene);
       }else
       {
           Scene* introScene = getIntroScene(victoryScene->m_renderer, victoryScene->m_window);
           changeState(introScene);
       }
    }else if (e == quitEv)
        isRunning = false;
}
static void update1(float dt)
{
    static float timer1 = 0;
    timer1 += dt;
    if (timer1 > 4000)
    {
        timer1 = 0;
        victoryScene->update = &update2;
        victoryScene->render = &render2;
    }
}
static void update2(float dt)
{
    static float timer2 = 0;
    timer2 += dt;
    myVars.fireWorksTimer +=dt;
    if (timer2 > 8000)
    {
        timer2 = 0;
        victoryScene->update = &update3;
        victoryScene->render = &render3;
    }

}
static void update3(float dt)
{
    static int n = 0;
    static float total = 0;
    total += dt;
    if ( total > 500)
    {
        total = 0;
        n = ( n + 1) % 2;
    }
    myVars.t3 = (n) ? myVars.t2 : NULL;

}
static void render1()
{
    int width,height;
    SDL_QueryTexture(myVars.t1, NULL, NULL, &width, &height);
    SDL_Rect rect = {512 - width / 2, 150, width, height};
    SDL_RenderCopy( victoryScene->m_renderer,myVars.t1, NULL,&rect );
}
static void render2()
{

    int emptyIndex = -99;
    for (int i = 0; i < 10; ++i)
    {
        if ( myVars.fireWorks[i].x == -99 )
            emptyIndex = i;
        else // render the fire work
        {
            SDL_Rect rect = { 120, 64, 32, 32}; // the position on the sprite sheet of the explosion image.
            SDL_RenderCopy(victoryScene->m_renderer, victoryScene->m_texture, &rect, &myVars.fireWorks[i]);
        }
    }
    // if no empty index, make and empty one randomly, else, create a fire work at the empty index
    if ( emptyIndex == -99 ) // no empty slots
    {
        int idx = rand() % 10;
        myVars.fireWorks[idx].x = -99;
    }else
    {
        if (myVars.fireWorksTimer > 400)
        {
            myVars.fireWorksTimer = 0;
           createFireWork(emptyIndex);
        }
    }
}
static void render3()
{
    int width,height;
    SDL_QueryTexture(myVars.t3, NULL, NULL, &width, &height);
    SDL_Rect rect = {512 - width / 2, 384 - height / 2, width, height};
    SDL_RenderCopy( victoryScene->m_renderer,myVars.t3, NULL,&rect );
}
static void onEnter()
{
    Mix_PlayChannel( -1, myVars.gSound, 0 ); // play a sound effect
    myVars.lowest_score = 500000;
    readScoreFile(); // just to get the lowest high score recoreded.
    //Open the fonts
    myVars.f1 = TTF_OpenFont( "pixelFont.ttf", 38 );
    SDL_Color color = { 0, 250, 150, 255 };
    SDL_Color color2 = { 255, 255, 0, 255 };

    loadFromRenderedTexture(&myVars.t1, "VICTORY!",myVars.f1, color ,victoryScene->m_renderer ); // creates a texture from a string, stores
    loadFromRenderedTexture(&myVars.t2, "YOU WIN!",myVars.f1, color2 ,victoryScene->m_renderer );

    // load image
    loadMedia();

    // set rects of fire works to 'empty'
    for ( int i = 0; i < 10; ++i )
      myVars.fireWorks[i].x = -99;
}
static void onExit()
{
    free(victoryScene);
    victoryScene = NULL;

    freeTexture(myVars.t1);
    freeTexture(myVars.t2);
    if (myVars.t3)
    freeTexture(myVars.t3);

    free(myVars.f1);
    myVars.f1 = NULL;
}
static void readScoreFile()
{
    char* buf = file_read("highScores.txt");
    myGets(NULL, NULL,true); // reset the fseek to 0;

    // find the lowest score in the file
    char* scoreString = malloc(12); // hold the string representing the score
    for ( int lineNumber = 1; lineNumber < 10; lineNumber += 2)// score are on odd line numbers
    {
        myGets(buf, scoreString,false);
        int score = atoi(scoreString);
        if (score < myVars.lowest_score)
            myVars.lowest_score = score;
    }
    free(scoreString);
    scoreString = NULL;
    fclose(myVars.fp);
}

static bool loadMedia()
{
    //Loading success flag
    bool success = true;
    //Load sprite sheet texture

    // get rid of pre-existing texture
    freeTexture(victoryScene->m_texture);
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
    victoryScene->m_texture = SDL_CreateTextureFromSurface( victoryScene->m_renderer, loadedSurface );
    if( !(victoryScene->m_texture) )
    {
        printf( "Unable to create texture from %s! SDL Error: %s\n", "invaders.png", SDL_GetError() );
        success = false;
    }

    //Get rid of old loaded surface
    SDL_FreeSurface( loadedSurface );

    return success;
}

// create a new fire work at index.
static void createFireWork(int index)
{
    int xPos = (rand() % 824) + 100;
    int yPos = (rand() % 568) + 100;
    myVars.fireWorks[index].x = xPos;
    myVars.fireWorks[index].y = yPos;
    myVars.fireWorks[index].w = 32;
    myVars.fireWorks[index].h = 32;

    // play the sound
    Mix_PlayChannel( -1, myVars.gPop, 0 );

}












