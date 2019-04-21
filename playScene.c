
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <SDL_image.h>
#include "base.h"
#include "alien.h"
#include "playScene.h"
#include "utills.h"
#include "hud.h"
#include "ship.h"
#include "bunker.h"
#include "wave.h"
#include "def.h"
#include "inputCtrl.h"
#include "introScene.h"
#include "stateMachine.h"
#include "pauseScene.h"
#include "scoreScene.h"
#include "victoryScene.h"

#define ALIEN_HIT_POINTS 10
#define MOTHER_HIT_POINTS 300
#define MAX_POINTS 500000
#define BUNKERS_LEFT_START 130
#define BUKERS_SPACING 230

void processEvents(inputEvent);
void update(float);
void render();
void onEnter();
void onExit();
bool loadMedia();
void checkCollisions(); // check what entity made contact with what entity
void playerHit(); // player got hit
void hitUpdate(float); // the alternative update function to run while player hit animation is going
void continueScene(); // continue scene after loss of player
void restartWave(); // wave dead, start a new one at a highe level
void resetBunkers(); // clear out old bunkers and make 4 new ones (after wave restart)
void checkAlienBunkerCol(); // check alien to bunker contact.
static void gameOver();
static void gameOverEvents(inputEvent); // the game over process events method
static void gameOverUpdate(float); // the game over update method.
static void gameOverRender(); // the game over render method.
static void readyEvents(); // an empty process events method to be run during 'ready' message
static void readyUpdate(float); // the update method to be run during the 'ready' message
static int getHighScore(); // read the high score from the file

Scene* playScene = NULL;
extern bool isRunning;
// one sample alien.
Base* myBase = NULL;
// the hud
Hud* myHud = NULL;
//mother ship
Ship* mysteryShip = NULL;
Bunker* Bariers[4]; // holds all the bunkers.
Wave* fleet = NULL; // fleet of aliens
static int levelNum = 0;
static int minScore = 500000; // the smallest score in the hall of fame.

Scene* getPlayScene(SDL_Renderer* rend, SDL_Window* win)
{
  playScene = malloc(sizeof(Scene));
  playScene->update = &readyUpdate;
  playScene->render = &render;
  playScene->onEnter = &onEnter;
  playScene->onExit = &onExit;
  playScene->processEvents = &readyEvents;
  playScene->m_renderer = rend;
  playScene->m_window = win;
  playScene->m_texture = NULL;
    playScene->m_next = NULL;
  return playScene;
}

void onEnter()
{
  srand((unsigned int)time(NULL));   // Initialization, should only be called once.
  loadMedia();
  myBase = getBase(playScene->m_renderer,playScene->m_texture);
  myHud = createHud(playScene->m_renderer,playScene->m_texture);
  myHud->m_hightScore = getHighScore();
  readyHud();
  fleet = getNewWave(0, playScene->m_renderer, playScene->m_texture, myBase);
  // create 4 bunkers
    for ( int i = 0; i < 4; ++i)
    {
        Bunker* newBuker = getNewBunker(playScene->m_renderer, playScene->m_window,BUNKERS_LEFT_START + i * BUKERS_SPACING);
        Bariers[i] = newBuker;
    }
}

void onExit()
{
  freeTexture(playScene->m_texture);
  if (myBase)
  myBase->cleanUp();
  myHud->cleanUp();
  free(myBase);
  myBase = NULL;
  freeWave();// frees the memory used by aliens.
  free(fleet);
  fleet = NULL;
  // free the bunkers
    for ( int i = 0; i < 4; ++i)
    {
        free(Bariers[i]);
        Bariers[i] = NULL;
    }
    free(playScene);
    playScene = NULL;
}

void processEvents(inputEvent e)
{
  if ( e == pauseEv )
  {
      Scene* pause = getPauseScene(playScene->m_renderer, playScene->m_window);
      pushState(pause);
      return;
  }else if (e == quitEv)
    isRunning = false;
  myBase->processEvents(e);
}

void update(float dtime)
{
    fleet->m_mysteryTime += dtime;
    fleet->update(dtime);
    myBase->update(dtime);
    myHud->update(dtime);
    if (mysteryShip)
    {
        mysteryShip->update(dtime);
        if (mysteryShip->m_destroyQueue)
        {
            cleanUpShip(); // frees up the memory used by mother ship
            mysteryShip = NULL;
        }
    }else // no mystery ship, maybe dispatch one.
    {
       if (fleet->m_mysteryTime >= 30000 && fleet->m_numAliens > 15)
       {
           int rnd = rand() % 2;
           flyDir dir = (rnd) ? LEFT : RIGHT;
           fleet->m_mysteryTime = 0;
           mysteryShip = getNewShip(dir, playScene->m_renderer, playScene->m_texture);
       }
    }
    checkCollisions();
}

void render()
{

    // render the bunkers
    for ( int i = 0; i < 4; ++i)
    {
        Bunker* temp = Bariers[i];
        Bariers[i]->render(temp);
    }
    myBase->render();
    fleet->render();
    myHud->render();
   if (mysteryShip)
       mysteryShip->render();

}

bool loadMedia()
{
  //Loading success flag
    bool success = true;
  //Load sprite sheet texture

 // get rid of pre-existing texture
    freeTexture(playScene->m_texture);
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
  playScene->m_texture = SDL_CreateTextureFromSurface( playScene->m_renderer, loadedSurface );
  if( !(playScene->m_texture) )
  {
    printf( "Unable to create texture from %s! SDL Error: %s\n", "invaders.png", SDL_GetError() );
    success = false;
  }

  //Get rid of old loaded surface
   SDL_FreeSurface( loadedSurface );

   return success;
}

// check what entity made contact with what entity
void checkCollisions()
{
  if (myBase->m_bullet)
  {
    // check if bullet hit alien
      if (isAlienHit(myBase->m_bullet->m_collider))
      {
        // alien is removed in wave unit, remove bullet here
          myBase->m_bullet->m_destroyQueue = true;
          myHud->m_score = min(myHud->m_score + ALIEN_HIT_POINTS, MAX_POINTS);
          fleet->m_numAliens -= 1; // one less bastard alien.
          if ( fleet->m_numAliens <= 0 )
          {
              if (myHud->m_score >= MAX_POINTS) // game is won!!!
              {
                  // change scene to victory scene
                  Scene* victory = getVictoryScene(playScene->m_renderer, playScene->m_window);
                  changeState(victory);
                  return;
               }
              // restart wave at a higher level.
              restartWave();
              // refresh the bunkers
              resetBunkers();
          }
      }
      // check if a bullet hit the mystery ship
      else if (mysteryShip)
      {
          if ( testCollision(mysteryShip->m_collider, myBase->m_bullet->m_collider))
          {
              mysteryShip->takeHit();
              myHud->m_score = min( myHud->m_score + MOTHER_HIT_POINTS, MAX_POINTS); // update the score for the hit.
              myBase->m_bullet->m_destroyQueue = true;
              myBase->m_canFire = true;
          }
      }
}

    if (fleet->m_bullet)
    {
        // check if base bullet hit alien bullet

        if ( (myBase->m_bullet) && (testCollision(fleet->m_bullet->m_collider, myBase->m_bullet->m_collider)))
        {
            bulletExplode(myBase->m_bullet); // change the image of base bullet to explosion and start half second time to removal.
            fleet->m_bullet->m_destroyQueue = true; // remove the alien bullet.
        }


        // check if base is hit by alien bullet.
      if (isBaseHit(myBase->m_collider))
      {
        playerHit(); // bullet gets destroyed in wave unit.
        if (mysteryShip)
        {
           cleanUpShip();
           mysteryShip = NULL;
        }
      }
    }

    // check if bunker hit by a bullet

    bool playerInRect = false; // player bullet in bunker rect.
    bool enemyInRect = false; // enemy bullet in bunker rect.

    for ( int i = 0; i < 4; ++i)
    {
        Bunker* temp = Bariers[i];
        SDL_Rect collide = { temp->m_posX, BUNKER_Y_POS, BUNKER_WIDTH, BUNKER_HEIGHT };
        if (myBase->m_bullet)
        {
          if (testCollision(collide, myBase->m_bullet->m_collider))
          {
              manipulate(temp, myBase->m_bullet);
              playerInRect = true;
          }
        }

        if (fleet->m_bullet)
        {
           if (testCollision(collide, fleet->m_bullet->m_collider))
           {
               manipulate(temp, fleet->m_bullet);
               enemyInRect = true;
           }
        }
    }
    if ( (myBase->m_bullet) && !playerInRect ) // if player bullet is not inside any of the bunkers
        bulletResumeNormalSpeed(myBase->m_bullet);

    if ( (fleet->m_bullet) && !enemyInRect ) // if alien bullet is not inside any of the bunkers
        bulletResumeNormalSpeed(fleet->m_bullet);

    // check alien to bunker collisions.
    int depth =  getBottomEdge() + 32; // 32 is alien height
    if ( depth >= BUNKER_Y_POS )
    {
        checkAlienBunkerCol();
        // and last check if an alien has touched down.
        if ( depth >= 720 ) // 720 is the floor height
        {
            myHud->m_lives = 0;
            playerHit();
        }
    }

}

// player got hit
void playerHit()
{
    baseHit();
    playScene->update = &hitUpdate;
}

// the alternative update function to run while player hit animation is going
void hitUpdate(float dt)
{
    static float hitTime = 0;
    hitTime += dt;
    myBase->update(dt);
    myHud->update(dt);
    if (hitTime >= 2000)
    {
        if (myBase->m_bullet)
        {
            destroyBullet(myBase->m_bullet);
            myBase->m_bullet = NULL;
        }

        if (fleet->m_bullet)
        {
            destroyBullet(fleet->m_bullet);
            fleet->m_bullet = NULL;
        }

        hitTime = 0; // for next time around player gets hit.
        free(myBase);
        myBase = NULL;
         myHud->m_lives -= 1; // lose one life.
        if (myHud->m_lives > 0)
        continueScene();
        else
            gameOver();
    }
}

// continue scene after loss of player
void continueScene()
{
    myBase = getBase(playScene->m_renderer,playScene->m_texture);
    playScene->update = &readyUpdate;
    playScene->processEvents = &readyEvents;
    readyHud(); // hud to display ready message
   // the wave needs an updated base pointer
    fleet->m_base = myBase;
}

 // wave dead, start a new one at a lower level
void restartWave()
{
    levelNum = (levelNum + 1 ) % 8; // after 7 drops, start at 0 again.
    freeWave();
    free(fleet);
    fleet = NULL;
    fleet = getNewWave(levelNum, playScene->m_renderer, playScene->m_texture, myBase);
    myBase->m_posX = 80; // base left limit.
    myBase->m_canFire = true;
}

// clear out old bunkers and make 4 new ones (after wave restart)
void resetBunkers()
{
    // free existing 4 bunkers and create 4 new ones.
    for ( int i = 0; i < 4; ++i)
    {
        freeTexture( Bariers[i]->m_newTexture );
        free(Bariers[i]);
        Bariers[i] = NULL;
        Bunker* newBuker = getNewBunker(playScene->m_renderer, playScene->m_window,BUNKERS_LEFT_START + i * BUKERS_SPACING);
        Bariers[i] = newBuker;
    }
}


// check alien to bunker contact.
void checkAlienBunkerCol()
{
    for ( int i = 0; i < 11; ++i)
    {
        Alien* temp1 = getButtonOfColoum(i);

        for ( int j = 0; j < 4; ++j)
        {
            SDL_Rect collide = { Bariers[j]->m_posX, BUNKER_Y_POS, BUNKER_WIDTH, BUNKER_HEIGHT };
            if (temp1 && testCollision(temp1->m_collider, collide))
                bunkerAlienContact(Bariers[j],temp1);
        }
    }
}

static void gameOver()
{
    playScene->update = &gameOverUpdate;
    playScene->processEvents = &gameOverEvents;
    playScene->render = &gameOverRender;
    myHud->gameOver();
}

 // the game over process events method
static void gameOverEvents(inputEvent e)
{
    if (e == fireEv)
    {
        Scene* nextScene = NULL;
        int score = myHud->m_score;
        if ( score > minScore)
            nextScene = getScoreScene(playScene->m_renderer, playScene->m_window, score);
        else
            nextScene = getIntroScene(playScene->m_renderer, playScene->m_window);

        changeState(nextScene);
    }else if ( e == quitEv )
        isRunning = false;
}

// the game over update method.
static void gameOverUpdate(float dt)
{

}

// the game over render method.
static void gameOverRender()
{
    // render the bunkers
    for ( int i = 0; i < 4; ++i)
    {
        Bunker* temp = Bariers[i];
        Bariers[i]->render(temp);
    }

    fleet->render();
    myHud->render();
}

// an empty process events method to be run during 'ready' message
static void readyEvents()
{

}

// the update method to be run during the 'ready' message
static void readyUpdate(float dt)
{
    static float timeToStart = 0;
    timeToStart += dt;
    if (timeToStart >= 2000)
    {
        timeToStart = 0; // for next time around.
        playScene->update = &update;
        playScene->processEvents = &processEvents;
        playScene->render = &render;
        unreadyHud(); // move hud out of ready mode.
    }
}

// read the high score from the file and the lowest of the hight scores.
static int getHighScore()
{
    int max = 0;
    char* buf = file_read("highScores.txt");
    myGets(NULL, NULL,true); // reset the fseek to 0;

    for ( int lineNumber = 0; lineNumber < 10; ++lineNumber)
    {
        char* num = malloc(15);
        myGets(buf, num, false);
        if ((lineNumber % 2)) // scores are on odd line numbers
        {
            int temp = atoi(num);
            if (temp > max)
                max = temp;
            if ( temp < minScore )
                minScore = temp;
        }
        free(num);
        num = NULL;
    }

    free(buf);
    buf = NULL;

    return max;
}



























