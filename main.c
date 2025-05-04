#include <SDL3/SDL_error.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  float bird_offset;
} AppState;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  AppState *as = (AppState *)SDL_calloc(1, sizeof(AppState));
  if (!as) {
    SDL_Log("Couldn't allocate memory: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  *appstate = as;
  SDL_SetAppMetadata("Flappy Bird", "1.0", "northmatrix.co.uk");
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  if (!SDL_CreateWindowAndRenderer("Flappy Bird", 640, 480, 0, &as->window,
                                   &as->renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_SetRenderDrawColor(as->renderer, 26, 26, 26, 1);
  SDL_RenderClear(as->renderer);
  SDL_RenderPresent(as->renderer);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) { return SDL_APP_CONTINUE; }

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
