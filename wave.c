// the wave of aliens
#include <stdlib.h>
#include "wave.h"
#include "utills.h"

#define WAVE_TOP_Y_POS 118
#define WAVE_RANK_SPACING 64
#define WAVE_FILE_SPACING 64
#define WAVE_MIN_X_POS 64
#define WAVE_MAX_X_POS PLAY_AREA_WIDTH -96
#define WAVE_NUM_RANKS 5
#define WAVE_NUM_FILES 11
#define WAVE_VERT_DROP 21
#define WAVE_HORIZONTAL_STEP 32

static void update(float);
static void render();
static void init(Alien*[WAVE_NUM_FILES][WAVE_NUM_RANKS], int); // init the wave;
static void moveWave(mvDir); // move the whole wave in a direction
static float getRightEdge(); // get the X pos of the right most alien in the wave
static float getLeftEdge(); // get the X.........
static void detrmineMvDir(); // determine and move the whole wave in the correct direction
static void loadSoundFX();
static Alien* getLowestInCol(int); // get the lowest alien in a colum.
static void realignRndColSelect(); // if found that a colum is empty, realign the avialable colums to chosoe from.


Wave* myWave = NULL;
Mix_Chunk* mvFx[4];
int colums[12]; // holds all the colums of aliens to choose from, +1 to indicate the last of the available colums.

Wave* getNewWave(int level ,SDL_Renderer* rend, SDL_Texture* tex, Base* theBase )
{
    myWave = malloc(sizeof(Wave));
    myWave->m_renderer = rend;
    myWave->m_texture = tex;
    myWave->m_totalTime = 0;
    myWave->m_numAliens = 55; // start with 55 aliens.
    myWave->m_bullet = NULL;
    myWave->m_mysteryTime = 0;
    myWave->m_mode = kRight;
    myWave->m_base = theBase;
    myWave->update = &update;
    myWave->render = &render;
    init(myWave->alArray, level);
    return myWave;
}

static void update(float dt)
{
  for ( int i = 0; i < 5; ++i )
    {
        for ( int j = 0; j < 11; ++j)
        {
            Alien* temp = myWave->alArray[j][i];
            if (temp)
            {
                temp->update(dt,temp);

            if (temp->m_destroyQueue) // time to remove this dead alien
                {
                    freeAlien(temp);
                    myWave->alArray[j][i] = NULL;
                    myWave->m_base->m_canFire = true;
                }
            }
        }
    }
    myWave->m_totalTime +=dt;
    int  timeToMove = max( myWave->m_numAliens * 18, 72 );
    if ( myWave->m_totalTime >= timeToMove ) // movement speed
    {
        detrmineMvDir();
        myWave->m_totalTime = 0;
    }

    // update the bullet if exists
    if (myWave->m_bullet)
    {
        myWave->m_bullet->update(dt, myWave->m_bullet);
        if (myWave->m_bullet->m_destroyQueue)
        {
            destroyBullet(myWave->m_bullet);
            myWave->m_bullet = NULL;
        }
    }else // no bullet, then fire one.
    {
        if (!myWave->m_base->m_isAmmune)
        waveFire();
    }
}

static void render()
{
    for ( int i = 0; i < 5; ++i )
    {
        for ( int j = 0; j < 11; ++j)
        {
            Alien* temp = myWave->alArray[j][i];
            if (temp)
                temp->render(temp);
        }
    }
    //render the bullet if it exists
    if (myWave->m_bullet)
        myWave->m_bullet->render(myWave->m_bullet);
}

static void init(Alien* alArray[WAVE_NUM_FILES][WAVE_NUM_RANKS], int levelNum)
{
    float xPos; // the starting x position of the first colum on the left.
    float yPos; // the starting y position of the first row.

    for ( int i = 0; i < WAVE_NUM_RANKS; ++i )
    {
        yPos = (float)( i * WAVE_RANK_SPACING + (WAVE_TOP_Y_POS + levelNum * WAVE_VERT_DROP) );
        for ( int j = 0; j < WAVE_NUM_FILES; ++j )
        {
            xPos = (float)(j * WAVE_FILE_SPACING + WAVE_MIN_X_POS );
            Alien* newAlien = getNewAlien(myWave->m_renderer, myWave->m_texture, xPos, yPos, rand() % 8 );
            myWave->alArray[j][i] = newAlien;
        }
    }
    loadSoundFX();
    // init the arrays for random selecting rows.
    for ( int i = 0; i < WAVE_NUM_FILES; ++i )
     colums[i] = i;
     colums[11] = -1;
}

 // is any of the aliens in the wave hit by a bullet
bool isAlienHit(SDL_Rect bullRect)
{
    for ( int i = 0; i < 5; ++i )
    {
        for ( int j = 0; j < 11; ++j)
        {
            Alien* temp = myWave->alArray[j][i];
            if (temp && testCollision(bullRect, temp->m_collider))
            {
                alienHit(temp);
                return true;
            }
        }
    }
    return false;
}

// is base hit by alien bullet
bool isBaseHit(SDL_Rect baseRect)
{
    if ( (myWave->m_bullet) && testCollision(myWave->m_bullet->m_collider, baseRect) )
    {
        free(myWave->m_bullet); // destroy the bullet directly.
        myWave->m_bullet= NULL;
         return true;
    }
    return false;
}

// the direction to move the wave
static void moveWave(mvDir dir)
{
    float dx,dy; // movement in x or y direction
    switch (dir)
    {
        case kRight:
            dx = WAVE_HORIZONTAL_STEP;
            dy = 0;
            break;

        case kLeft:
            dx = -WAVE_HORIZONTAL_STEP;
            dy = 0;
            break;

        case kDown:
            dx = 0;
            dy = WAVE_VERT_DROP;
            break;
    }
    for ( int i = 0; i < WAVE_NUM_RANKS; ++i )
    {
        for ( int j = 0; j < WAVE_NUM_FILES; ++j )
        {
           Alien* temp = myWave->alArray[j][i];
           if (temp)
           {
               temp->m_posX += dx;
               temp->m_posY += dy;
               alienFrameChage(temp); // the animation frame changes only with alien movments
           }
        }
    }
 }



//free the memory used by the wave.
void freeWave()
{
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 11; ++j)
        {
            if (myWave->alArray[j][i])
            {
                freeAlien(myWave->alArray[j][i]);
                myWave->alArray[j][i] = NULL;
            }
        }
    }
    myWave->m_base = NULL;
    // free the sound effects
    Mix_FreeChunk( mvFx[0] );
    Mix_FreeChunk( mvFx[1] );
    Mix_FreeChunk( mvFx[2] );
    Mix_FreeChunk( mvFx[3] );
}

// helper function to get the X pos of the right most alien in the wave
static float getRightEdge()
{
    for ( int i = 10; i >= 0; --i)
    {
        for ( int j = 0; j < 5; ++j )
        {
            Alien* temp = myWave->alArray[i][j];
            if (temp)
                return temp->m_posX;
        }
    }
    return -1; // means no aliens left in the wave
}

// get the X pos of the left most alien in the wave
static float getLeftEdge()
{
    for ( int i = 0; i < 11; ++i)
    {
        for ( int j = 0; j < 5; ++j )
        {
            Alien* temp = myWave->alArray[i][j];
            if (temp)
                return temp->m_posX;
        }
    }
    return -1; // none left
}

// get the Y pos of the Bottom most alien in the wave
float getBottomEdge()
{
    for ( int j = 4; j >= 0; --j )
    {
        for ( int i = 0; i < 11; ++i )
        {
            Alien* temp = myWave->alArray[i][j];
            if (temp)
                return temp->m_posY;
        }
    }
    return -1; // none left
}

 // determine and move the whole wave in the correct direction and play sound.
static void detrmineMvDir()
{
  static int soundNum = 0;
  if ( myWave->m_mode == kRight ) // wave is in right moving mode
  {
      if (getRightEdge() >= WAVE_MAX_X_POS)
      {
          moveWave(kDown);
          myWave->m_mode = kLeft; // change direction
    }else // keep moving right
          moveWave(kRight);
  }else
  {
      if (getLeftEdge() <= WAVE_MIN_X_POS)
      {
          moveWave(kDown);
          myWave->m_mode = kRight; // change direction
      }else // keep moving left
          moveWave(kLeft);
  }
   // play the right sound in sequence
    Mix_PlayChannel( -1, mvFx[soundNum], 0 );
    soundNum = (soundNum + 1) % 4;
}


// load the sounds effects for the wave.
static void loadSoundFX()
{
    //Load sound effects
    mvFx[0] = Mix_LoadWAV( "soundFX/fastinvader1.wav" );
    mvFx[1] = Mix_LoadWAV( "soundFX/fastinvader2.wav" );
    mvFx[2] = Mix_LoadWAV( "soundFX/fastinvader3.wav" );
    mvFx[3] = Mix_LoadWAV( "soundFX/fastinvader4.wav" );
}

// have wave fire a bullet from a random alien if no other alien bullet is on scree.
void waveFire()
{
    // pick a random colum.
    // first find the index in colums array for the range of colums available
    int index = 0;
    while ( colums[index] != -1 ) ++index;

    int rnd = rand() % index;
    // now check if that colum is actually not empty.
    Alien* temp = getLowestInCol(colums[rnd]);
    if ( temp == NULL) // thats an empty colum, realign the array of available colums.
    {
        realignRndColSelect();
        // run the whole sequence again, this time it should find an available colum.
        index = 0;
        while ( colums[index] != -1 ) ++index;
        rnd = rand() % index;
        temp = getLowestInCol(colums[rnd]);
    }
     if (!temp->m_isHit) // exploding alien should not fire.
    myWave->m_bullet = temp->fire(temp);
}
// get the lowest alien in a colum.
static Alien* getLowestInCol(int col)
{
    for ( int r = 4; r >= 0; --r )
    {
        if (myWave->alArray[col][r])
            return myWave->alArray[col][r];
    }
    return NULL; // the colum is empty.
}

// if found that a colum is empty, realign the avialable colums to choose from.
static void realignRndColSelect()
{
    int index = 0;
    for ( int i = 0; i < 11; ++i )
    {
       if (getLowestInCol(i)) // is there an alien left in that coloum
       {
           colums[index] = i;
           ++index;
       }
    }
    colums[index] = -1; // signifies the end of available colums.
}

// get the buttom alien of a coulum (if it exists)
Alien* getButtonOfColoum(int col)
{
    for ( int j = 4; j >= 0; --j )
    {
        Alien* temp = myWave->alArray[col][j];
        if (temp)
            return temp;
    }

    return NULL;
}




















