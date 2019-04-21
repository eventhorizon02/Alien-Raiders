
#include <SDL.h>
#include "inputCtrl.h"

const Uint8 *state;
//Analog joystick dead zone
const int JOYSTICK_DEAD_ZONE = 8000;
//Game Controller 1 handler
SDL_Joystick* gGameController = NULL;

void initCtrl()
{
   state = SDL_GetKeyboardState(NULL);

    //Check for joysticks
    if( SDL_NumJoysticks() > 0 )
    {
        //Load joystick
        gGameController = SDL_JoystickOpen( 0 );
    }

}

// which event happended
inputEvent getEvent()
{
    SDL_Event event;
    inputEvent myEvent = noEv;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                myEvent = quitEv;
                break;

            case SDL_KEYDOWN:
            {
                if ( (event.key.keysym.sym) == SDLK_SPACE )
                    myEvent = fireEv;
                else if ( (event.key.keysym.sym) == SDLK_ESCAPE )
                    myEvent = pauseEv;
                else if ( (event.key.keysym.sym) == SDLK_q )
                    myEvent = quitEv;
                else if  ( (event.key.keysym.sym) == SDLK_BACKSPACE )
                    myEvent = delEv;
                else if ( (event.key.keysym.sym) == SDLK_RETURN)
                    myEvent = entEv;
                break;
            }
            case SDL_JOYBUTTONDOWN:
            {
                myEvent = fireEv;
                break;
            }
        }
    }
    return myEvent;
}

 // is a state
bool isState(inputStates myState)
{
    static int map[2] = { SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT };
    if (state[map[myState]])
        return true;
    int xMove = SDL_JoystickGetAxis(gGameController, 0);
    if ( myState == mvLeftSt )
    {
       if (xMove < -JOYSTICK_DEAD_ZONE)
           return true;
    }else if ( myState == mvRightSt )
    {
        if (xMove > JOYSTICK_DEAD_ZONE)
            return true;
    }
    return false;
}

// close input control
void closeCtrl()
{
    //Close game controller
    SDL_JoystickClose( gGameController );
    gGameController = NULL;
}








