// a collection of utillity functions

#include <SDL.h>
#include <SDL_ttf.h>
#include "def.h"


float clamp(float x, float a, float b); //keep x between a and b
bool testCollision( SDL_Rect, SDL_Rect );
void freeTexture(SDL_Texture*); // free a texture.
char* int_to_ascii(int); // convert and integer to a string.
int max(int,int); // get the bigger of the two.
int min(int,int); // get the smaller of the two.
void loadFromRenderedTexture(SDL_Texture**, const char*, TTF_Font*, SDL_Color ,SDL_Renderer* ); // create a texture from a string.
char* file_read(const char*);// read an entire file into a buffer
void myGets(char*, char*, bool); // read a line from a char array ( lines separated by '\n' )
