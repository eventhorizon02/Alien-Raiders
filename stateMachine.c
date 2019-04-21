#include <stdlib.h>
#include "stateMachine.h"
#include "inputCtrl.h"

static void update(float dt);
static void processEvents();
static void render();

fsm* machine = NULL;

fsm* getNewMachine(SDL_Renderer* rend, SDL_Window* win)
{
    machine = malloc(sizeof(fsm));
    machine->render = &render;
    machine->update = &update;
    machine->processEvents = &processEvents;
    machine->m_head = NULL;
    machine->m_renderer = rend;
    machine->m_window = win;
    return machine;
}

void changeState(Scene* newState)
{
    Scene* current = machine->m_head;
    if (current) // there is a working scene
        current->onExit();

    machine->m_head = newState;
    machine->m_head->onEnter();


}

void pushState(Scene* newState)
{
    Scene* current = machine->m_head;
    newState->onEnter();
    machine->m_head = newState;
    newState->m_next = current;
}

// pop the top scene
void pop()
{
    // being a bit lazy and assuming without checking that there is a 'current' scene in the list.

    Scene* current = machine->m_head;

    if (current->m_next)
      machine->m_head = current->m_next;
    else
     machine->m_head = NULL;

    current->onExit();

}

static void update(float dt)
{
    machine->m_head->update(dt);
}

static void processEvents()
{
    inputEvent e = getEvent();
    machine->m_head->processEvents(e);
}

static void render()
{
    // in case the scene is paused there are going to be to scenes in the list,
    // so render the 'bottom' one first.

    //Clear screen
    SDL_SetRenderDrawColor( machine->m_renderer, 0x00, 0x0, 0x0, 0x0 );
    SDL_RenderClear( machine->m_renderer );

    Scene* second = machine->m_head->m_next;
    if (second)
        second->render();
        machine->m_head->render();

    //Update screen
    SDL_RenderPresent( machine->m_renderer );
}









