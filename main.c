#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_main.h>

#define WIDTH 640
#define HEIGHT 480

#define BIRD 50
#define BIRD_X_OFFSET 20

typedef struct {
  float x_offset;
  float y_offset;
} Pipe;

typedef struct {
  float bird_offset;
  Pipe pipes[2];
} GameCtx;

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  GameCtx *game_context;
} AppState;

void init_game_context(GameCtx **game_context) {
  *game_context = SDL_calloc(1, sizeof(GameCtx));
  if (*game_context) {
    (*game_context)->bird_offset = (HEIGHT / 2.0) - (BIRD / 2.0);
  } else {
    SDL_Log("Memory allocation failed for game context: %s", SDL_GetError());
    return;
  }
}

void draw_pipe(SDL_Renderer *renderer, float x, float y) {
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 1);
  SDL_FRect brect = {x, y, BIRD, HEIGHT};
  SDL_FRect trect = {x, y - BIRD * 2, BIRD, HEIGHT};
  SDL_RenderFillRect(renderer, &brect);
  SDL_RenderFillRect(renderer, &trect);
}

void draw_game_context(SDL_Renderer *renderer, GameCtx *game_context) {
  SDL_SetRenderDrawColor(renderer, 26, 26, 26, 1);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);
  SDL_FRect rect = {BIRD_X_OFFSET, game_context->bird_offset, BIRD, BIRD};
  SDL_RenderFillRect(renderer, &rect);
  for (int i = 0; i < 2; i++) {
    draw_pipe(renderer, game_context->pipes[i].x_offset,
              game_context->pipes[i].y_offset);
  }
  SDL_RenderPresent(renderer);
}

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
  if (!SDL_CreateWindowAndRenderer("Flappy Bird", WIDTH, HEIGHT, 0, &as->window,
                                   &as->renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  init_game_context(&as->game_context);
  SDL_SetRenderDrawColor(as->renderer, 26, 26, 26, 1);
  SDL_RenderClear(as->renderer);
  SDL_RenderPresent(as->renderer);
  as->game_context->pipes[0].x_offset = WIDTH / 2;
  as->game_context->pipes[1].x_offset = (WIDTH / 2) - 100;

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *as = appstate;
  draw_game_context(as->renderer, as->game_context);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
