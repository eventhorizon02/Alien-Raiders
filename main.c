#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "def.h"
#include "stateMachine.h"
#include "introScene.h"


float getDeltaTime(); // get the ellapsed time since last upate.

//Starts up SDL and creates window
bool init();

//Frees media and shuts down SDL
void closeAll();
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

bool isRunning = true;

int main(int argc, char *argv[])
{
   init();
   initCtrl(); // init the input control
   fsm* myMachine = getNewMachine(renderer, window);
   Scene* intro = getIntroScene(renderer, window);
   pushState(intro);
    while(isRunning)
    {
        myMachine->processEvents();
        myMachine->update(getDeltaTime());
        myMachine->render();
    }

    closeAll();

  return 0;
}

float getDeltaTime()
{
  static long long  last = 0;
  long long now = SDL_GetTicks();
  float deltaTime = (float)(now -last);
  last = now;
  return deltaTime;
 }

// initialize window and renderer. and keyboard state
bool init()
{
  isRunning = true;
  bool success = true;
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
 window=SDL_CreateWindow("Alien Raiders",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,PLAY_AREA_WIDTH,PLAY_AREA_HEIGHT, SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
  //Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;
  if( !( IMG_Init( imgFlags ) & imgFlags ) )
  {
    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
    success = false;
  }

  //Initialize SDL_mixer
 if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 1024 ) < 0 )
 {
    printf( "SDL_mixer could not be initialized! SDL_mixer Error: %s\n", Mix_GetError() );
    success = false;
 }
  // init ttf
    if( TTF_Init() == -1 )
    {
        printf( "SDL_ttf could not be initialized! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }

  return success;
}


// free everyting
void closeAll()
{
  //Destroy window
    SDL_DestroyWindow( window);
    window = NULL;
    SDL_DestroyRenderer( renderer );
  renderer = NULL;
//Quit SDL subsystems
   IMG_Quit();
   SDL_Quit();
   Mix_Quit();
   closeCtrl(); // close input control
}




















