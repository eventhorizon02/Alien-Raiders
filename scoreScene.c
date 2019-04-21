
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdlib.h>
#include <string.h>
#include "scoreScene.h"
#include "introScene.h"
#include "inputCtrl.h"
#include "stateMachine.h"
#include "utills.h"

static void processEvents(inputEvent);
static void afterEvents(inputEvent); // the process events for after entering your name
static void update(float);
static void render();
static void onEnter();
static void onExit();
static void readScoreFile();
static void buildNameTex(); // create the high score name textures
static void sortScores(int); // sort the scores adding the new given high score
static void rebuildSigleTex(int,char);// re-create the texture that is being modified.
static void writeFile(); // write the scores to file.

Scene* scoreScene = NULL;
extern bool isRunning;

static int newScore = 0;

typedef  struct labels
{
    SDL_Texture* t1;
    SDL_Texture* t2;
    SDL_Texture* name_tex[5]; // the high scores name textures
    TTF_Font* f1;
    char* names[5]; // holds the names of the hi scorers
    int scores[5]; // holds the corresponding socres
    int rank; // the rank of the score to be changed.
    char* label[5]; // the strings that are used to create the name_tex
    int letter_pos; // the position in the text that is being edited
    char letter; // the letters cycled through
    float total_time; // a timer to help slow the cycling of letters
    Mix_Chunk* gClick; // click sound when cycling through letters.

}Labels;

static Labels myLabels;

Scene* getScoreScene(SDL_Renderer* rend, SDL_Window* win, int score)
{
    scoreScene = malloc(sizeof(Scene));
    scoreScene->m_renderer = rend;
    scoreScene->m_window = win;
    scoreScene->onEnter = &onEnter;
    scoreScene->onExit = &onExit;
    scoreScene->update = &update;
    scoreScene->render = &render;
    scoreScene->processEvents = &processEvents;
    scoreScene->m_next = NULL;
    scoreScene->m_texture = NULL;
    newScore = score;
    return scoreScene;
}

static void onEnter()
{
    //Open the fonts
    myLabels.f1 = TTF_OpenFont( "pixelFont.ttf", 38 );
    SDL_Color color = { 0, 250, 150, 255 };
    SDL_Color color2 = { 255, 255, 0, 255 };

    loadFromRenderedTexture(&myLabels.t1, "YOU HAVE ACHIEVED A HIGH SCORE",myLabels.f1, color ,scoreScene->m_renderer ); // creates a texture from a string, stores
    loadFromRenderedTexture(&myLabels.t2, "ENTER YOUR NAME",myLabels.f1, color2 ,scoreScene->m_renderer );

   // read the high scores files.
    readScoreFile();

    // socres need to be sorted and presented
    sortScores(newScore);

    // build the name textures
    buildNameTex();

    myLabels.letter_pos = 2; // the first position to edit letters
    myLabels.total_time = 0;
    myLabels.letter = 0; // start at 'A'

    //Load sound effect
    myLabels.gClick = Mix_LoadWAV( "soundFX/tik.wav" );
}

static void onExit()
{
    for (int i = 0; i < 5; ++i)
    {
        free(myLabels.label[i]);
        myLabels.label[i] = NULL;
        freeTexture(myLabels.name_tex[i]);
        myLabels.name_tex[i] = NULL;
        free(myLabels.names[i]);
        myLabels.names[i] = NULL;
    }
    free(scoreScene);
    scoreScene = NULL;
}

static void render()
{

    int width,height;

    SDL_QueryTexture(myLabels.t1, NULL, NULL, &width, &height);
    SDL_Rect rect1 = {512 - width / 2, 150, width, height};
    SDL_RenderCopy( scoreScene->m_renderer,myLabels.t1, NULL,&rect1 );

    SDL_QueryTexture(myLabels.t2, NULL, NULL, &width, &height);
    SDL_Rect rect2 = {512 - width / 2, 250, width, height};
    SDL_RenderCopy( scoreScene->m_renderer,myLabels.t2, NULL,&rect2 );


    // render the hight score names
    for ( int i = 0; i < 5; ++i )
    {
        SDL_QueryTexture(myLabels.name_tex[i], NULL, NULL, &width, &height);
        SDL_Rect rect = {512 - width / 2, 350 + i * 50, width, height};
        SDL_RenderCopy( scoreScene->m_renderer,myLabels.name_tex[i], NULL,&rect );
    }

}

static void processEvents(inputEvent e)
{
    if (e == fireEv)
    {
        *( myLabels.names[myLabels.rank] + myLabels.letter_pos - 2) = myLabels.letter + 'A'; // save the new letter to names
        myLabels.letter_pos = min((myLabels.letter_pos + 1), 11);
        rebuildSigleTex(myLabels.letter_pos,'A');
        myLabels.letter = 0;
    }else if (e == delEv)
    {
       rebuildSigleTex(myLabels.letter_pos, '-');
       *( myLabels.names[myLabels.rank] + myLabels.letter_pos - 2) = '\0';
       myLabels.letter_pos = max(2, myLabels.letter_pos -1);
       myLabels.letter = *(myLabels.names[myLabels.rank] + myLabels.letter_pos - 2);
    }else if (e == entEv)
    {

        *( myLabels.names[myLabels.rank] + myLabels.letter_pos - 2) = myLabels.letter + 'A'; // save the new letter to names
        for ( int i = myLabels.letter_pos + 1; i < 12; ++i)
        {
            rebuildSigleTex(i,' '); // remove the remaining dashes
           *( myLabels.names[myLabels.rank] + i - 2) = '\0'; // and in the names as well
        }

        writeFile(); // write scores to file
        scoreScene->processEvents = &afterEvents;
    }else if (e == quitEv)
     isRunning = false;


    if ( myLabels.total_time >= 300 )
    {
        if ( isState(mvRightSt) )
        {
             Mix_PlayChannel( -1, myLabels.gClick, 0 ); // play a sound effect
            myLabels.letter =  ( (myLabels.letter + 1) % 26 );
            rebuildSigleTex(myLabels.letter_pos, myLabels.letter + 'A');
            myLabels.total_time = 0;

        }else if (isState(mvLeftSt))
        {
            Mix_PlayChannel( -1, myLabels.gClick, 0 ); // play a sound effect
            myLabels.letter -= 1;
            if (myLabels.letter < 0)
                myLabels.letter = 25;
            rebuildSigleTex(myLabels.letter_pos, myLabels.letter + 'A');
            myLabels.total_time = 0;
        }
    }
}

static void update(float dt)
{
    myLabels.total_time += dt;
    if (myLabels.total_time > 500000)
        myLabels.total_time = 0;
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
            myLabels.names[evens] = malloc(12);
            myGets(buf, myLabels.names[evens],false);
            unsigned long len = strlen(myLabels.names[evens]);
            *(myLabels.names[evens] + len - 1) = '\0'; // get rid of the new line char
            ++evens;
        }else
        {
            char* num = malloc(8);
            myGets(buf, num, false);
            myLabels.scores[odds] = atoi(num);
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

    for ( int i = 0; i < 5; ++i )
    {
        char* label = malloc(21); // 21 for max player name is 10 plus 6 for max score of 500,000 points plus spaces and sequence.

        for ( int j = 0; j < 21; ++j )
            *(label + j ) = ' ';

        *(label + 20) = '\0';
        *(label) = i + 1 + '0';
        *(label + 1) = ':';
        // add the name to the label
        char* name = myLabels.names[i];
        unsigned long len = strlen(myLabels.names[i]);
        for ( int counter = 0; counter < len; ++counter)
        {
            *(label + counter + 2) = *name;
            ++name;
        }
        // add the score to the label
        char* str = int_to_ascii(myLabels.scores[i]);
        len = strlen(str);
        for ( int counter = 0; counter < len; ++ counter )
            *(label + counter + 13) = *(str + counter); // 13 is for a max name length of 10 plus sequence number plus ':' plus a space
        free(str);// the int_to_ascii alloces memory that needs to be freed.
        str = NULL;
        loadFromRenderedTexture(&myLabels.name_tex[i], label,myLabels.f1, color ,scoreScene->m_renderer );
        myLabels.label[i] = label; // need to keep track of it so it can be modified later
    }
    rebuildSigleTex(2, 'A'); // show an A at the first position
}

 // sort the scores adding the new given high score
static void sortScores(int newHigh)
{
  // this assumes the scores are already in a descending order
  // and that the passed new score is at least bigger than the smallest score in the hall of fame.
    for ( int i = 0; i < 5; ++i )
    {
       if ( newHigh > myLabels.scores[i] )
       {
           myLabels.rank = i;
           break;
       }
    }
    // at this point rank holds the place where the new score should sit, everything bellow that needs to move down one place
    // and the bottom score will be dropped.
    for ( int i = 4; i > myLabels.rank; --i )
    {
        myLabels.scores[i] = myLabels.scores[i-1];
        strcpy( myLabels.names[i],myLabels.names[i-1]);
    }
    // now place the new score in the right spot
    myLabels.scores[myLabels.rank] = newHigh;
    strcpy(myLabels.names[myLabels.rank], "----------");
}

// re-create the texture that is being modified.
static void rebuildSigleTex(int pos,char ch)
{
    SDL_Color color = { 255, 255, 255, 255 }; // white color
   // *( myLabels.label[myLabels.rank] + pos) = ch;
    char* temp = myLabels.label[myLabels.rank];
    *(temp + pos) = ch;

    loadFromRenderedTexture(&myLabels.name_tex[myLabels.rank], temp,myLabels.f1, color ,scoreScene->m_renderer );// recreate the texture.
}

// the process events for after entering your name
static void afterEvents(inputEvent e)
{
    if (e == fireEv)
    {
        Scene* introScene = getIntroScene(scoreScene->m_renderer, scoreScene->m_window);
        changeState(introScene);
    }else if ( e == quitEv )
        isRunning = false;
}

 // write the scores to file.
static void writeFile()
{
    SDL_RWops *fp = SDL_RWFromFile("highScores.txt", "w");
    char* buf = malloc(120);
    int bufIdx = 0; // the buf array index
    if (fp != NULL)
    {
      for ( int i = 0; i < 5; ++i)
      {

          int nameIdx = 0; // the name label index
          char c;
          while ( (c = *(myLabels.names[i] + nameIdx)) != '\0' )
          {
              *(buf + bufIdx) = c;
              ++bufIdx;
              ++nameIdx;
          }
          *(buf + bufIdx) = '\n';
          ++bufIdx;

          char* tempNum = malloc(8);
          strcpy(tempNum, int_to_ascii(myLabels.scores[i]));
          nameIdx = 0;
          while ( (c = *(tempNum + nameIdx)) != '\0' )
          {
              *(buf + bufIdx) = c;
              ++bufIdx;
              ++nameIdx;
          }

          *(buf + bufIdx) = '\n';
          ++bufIdx;
          free(tempNum);
          tempNum = NULL;
      }
        *(buf + bufIdx) = '\0';
    }

    size_t len = SDL_strlen(buf);
    SDL_RWwrite(fp, buf, 1, len);
    SDL_RWclose(fp);
    fp = NULL;
    free(buf);
    buf = NULL;
}

























