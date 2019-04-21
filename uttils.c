#include <stdlib.h>
#include "utills.h"


float clamp(float x, float a, float b) // keep x between a and b
{
  return x < a ? a : ( x > b ? b : x);
}

// check if two rects are touching.
bool testCollision( SDL_Rect p, SDL_Rect q )
{
    //The sides of the rectangles
    int leftP, leftQ, rightP, rightQ, topP, topQ, bottomP, bottomQ;

    //Calculate the sides of rect P
    leftP= p.x;
    rightP = p.x + p.w;
    topP = p.y;
    bottomP= p.y + p.h;

    //Calculate the sides of rect Q
    leftQ = q.x;
    rightQ = q.x + q.w;
    topQ = q.y;
    bottomQ = q.y + q.h;
    //If any of the sides from P are outside of Q
    if( bottomP <= topQ )
      return false;

    if( topP >= bottomQ )
      return false;

    if( rightP <= leftQ )
      return false;

    if( leftP >= rightQ )
      return false;

    //If none of the sides from P are outside Q then they touch
    return true;
}

void freeTexture(SDL_Texture* tex)
{
    //Free texture if it exists
    if( tex )
    {
        SDL_DestroyTexture( tex );
        tex= NULL;
    }
}

// convert and integer to a string.
char* int_to_ascii(int num)
{
    int count = (num == 0) ? 1 : 0;
    // get the length of the number first
    int numCpy = num;
    while (numCpy)
    {
        numCpy /= 10;
        ++count;
    }
    char* str = malloc(count + 1); // +1 for the null terminator.
    str[count] = '\0';
    --count;
    str[0] = '0';
    while(num)
    {
        str[count] = (num % 10) + '0';
        num /= 10;
        --count;
    }
    // caller of this function has to release the memory allocated.

    return str;
}

// get the bigger of the two.
int max(int x, int y)
{
    return ( x >= y ) ? x : y;
}

int min(int x, int y)
{
    return ( x <= y ) ? x : y;
}

// create a texture from a string.
void loadFromRenderedTexture(SDL_Texture** tex, const char* str, TTF_Font* gFont, SDL_Color color ,SDL_Renderer* rend)
{
    // free pre exitsting texture.
    freeTexture(*tex);

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, str, color );
    if( textSurface == NULL )
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    else
    {
        //Create texture from surface pixels
        *tex = SDL_CreateTextureFromSurface( rend, textSurface );
        if( tex == NULL )
            printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }
}

// read an entire file into a buffer
char* file_read(const char* filename)
{
    SDL_RWops *rw = SDL_RWFromFile(filename, "rb");
    if (rw == NULL) return NULL;

    Sint64 res_size = SDL_RWsize(rw);
    char* res = (char*)malloc(res_size + 1);

    Sint64 nb_read_total = 0, nb_read = 1;
    char* buf = res;
    while (nb_read_total < res_size && nb_read != 0)
    {
        nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
        nb_read_total += nb_read;
        buf += nb_read;
    }
    SDL_RWclose(rw);
    if (nb_read_total != res_size)
    {
        free(res);
        return NULL;
    }

    res[nb_read_total] = '\0';
    return res;
}

// read a line from a char array ( lines separated by '\n' )
void myGets(char* arr, char* buf, bool reset)
{
    static int index = 0; // the position in the arr

    if (reset) // kind of like reseting the position of fseek
    {
        index = 0;
        return; // it's just a reset call, no reading to do.
    }

    int bufIndx = 0; // the position in buf
    // stick in buf each char up to including the '\n'
    while ( *(arr + index) != EOF)
    {
        char c = *(arr + index);
        *(buf + bufIndx) = c;
        bufIndx++;
        index++;
        if (c == '\n')
        {
           *(buf + bufIndx) = '\0';
           return;
        }
    }
}















