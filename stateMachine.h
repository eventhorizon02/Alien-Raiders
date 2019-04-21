
#ifndef __stateMachine__
#define __stateMachine__

#include <SDL.h>
#include <stdio.h>
#include "def.h"

struct finiteStateMachine
{
    Scene* m_head; // the state machine is implemented as a linked list of scenes.
    SDL_Renderer* m_renderer;
    SDL_Window *m_window;
    void(*update)(float);
    void(*render)();
    void(*processEvents)();
};

typedef struct finiteStateMachine fsm;

fsm* getNewMachine(SDL_Renderer*, SDL_Window*);
void pushState(Scene*);
void pop();
void changeState(Scene*);



#endif /* stateMachine_h */
