// definitions and globlas.

#ifndef __def__
#define __def__

#include <SDL.h> //**************** remove after creating state machine *****************************************
#include "inputCtrl.h"

#define PLAY_AREA_WIDTH 1024
#define PLAY_AREA_HEIGHT 768

struct scene
{
  SDL_Renderer* m_renderer;
  SDL_Texture* m_texture;
  SDL_Window* m_window;
  void(*update)(float);
  void(*render)();
  void(*processEvents)(inputEvent);
  void(*onEnter)();
  void(*onExit)();
  struct scene* m_next;
};

typedef struct scene Scene;

#endif

