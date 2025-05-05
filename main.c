#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#define WIDTH 640
#define HEIGHT 480

#define BIRD 50
#define BIRD_X_OFFSET 75
#define JUMP 0.15
#define GRAVITY 0.9
#define BUTTON_DELAY 100

typedef struct {
  float x_offset;
  float y_offset;
} Pipe;

typedef struct {
  float bird_offset;
  Pipe pipes[2];
  float acceleration;
  bool is_alive;
} GameCtx;

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  GameCtx *game_context;
  double last_step;
  uint64_t last_press_time;
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

bool check_collisions(GameCtx *game_context) {
  float bird_bottom_right = BIRD_X_OFFSET + BIRD;
  float bird_bottom_left = BIRD_X_OFFSET;

  float bird_top = game_context->bird_offset;
  float bird_bottom = game_context->bird_offset + BIRD;

  if (bird_top < 0 || bird_bottom > HEIGHT) {
    return true;
  }
  for (int i = 0; i < 2; i++) {
    float pipe_left = game_context->pipes[i].x_offset;
    float pipe_right = game_context->pipes[i].x_offset + BIRD;
    // Now we check if we are collding horizontally
    bool is_horizontal_overlap_right =
        (bird_bottom_right > pipe_left) && (bird_bottom_right < pipe_right);
    bool is_horizontal_overlap_left =
        (bird_bottom_left > pipe_left) && (bird_bottom_left < pipe_right);
    if (is_horizontal_overlap_left || is_horizontal_overlap_right) {
      float pipe_bottom = game_context->pipes[i].y_offset;
      float pipe_top = game_context->pipes[i].y_offset - BIRD * 3.5;
      bool is_vertical_overlap =
          (bird_top > pipe_top) && (bird_bottom < pipe_bottom);
      if (!is_vertical_overlap) {
        return true;
      }
    }
  }
  return false;
}

void draw_pipe(SDL_Renderer *renderer, float x, float y) {
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 1);
  SDL_FRect brect = {x, y, BIRD, HEIGHT};
  SDL_FRect trect = {x, y - HEIGHT - BIRD * 3.5, BIRD, HEIGHT};
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
  as->last_step = SDL_GetPerformanceCounter();
  init_game_context(&as->game_context);
  as->game_context->is_alive = true;
  as->last_press_time = 0; // SDL_GetPerformanceCounter();
  as->game_context->pipes[0].x_offset = WIDTH + WIDTH * 0.75;
  as->game_context->pipes[0].y_offset = 200.0;
  as->game_context->pipes[1].x_offset = WIDTH + WIDTH * 0.25;
  as->game_context->pipes[1].y_offset = 400.0;
  SDL_SetRenderDrawColor(as->renderer, 26, 26, 26, 1);
  as->game_context->acceleration = -JUMP;
  SDL_RenderClear(as->renderer);
  SDL_RenderPresent(as->renderer);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  AppState *as = (AppState *)(appstate);
  uint64_t current_time = SDL_GetPerformanceCounter();
  double delta_time = (double)(current_time - as->last_press_time) /
                      SDL_GetPerformanceFrequency() *
                      1000.0; // Convert to milliseconds
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (event->key.scancode == SDL_SCANCODE_SPACE &&
        delta_time > BUTTON_DELAY) {
      as->game_context->acceleration = -JUMP;
      as->last_press_time = current_time;
    }
    if (event->key.scancode == SDL_SCANCODE_R) {
      as->game_context->pipes[0].x_offset = WIDTH + WIDTH * 0.75;
      as->game_context->pipes[0].y_offset = 200.0;
      as->game_context->pipes[1].x_offset = WIDTH + WIDTH * 0.25;
      as->game_context->pipes[1].y_offset = 400.0;
      as->game_context->is_alive = true;
      as->game_context->bird_offset = HEIGHT / 2.0 - BIRD / 2.0;
      as->game_context->acceleration = -JUMP;
    }
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *as = appstate;
  // Timing
  Uint64 now = SDL_GetPerformanceCounter();
  double deltaTime =
      (double)(now - as->last_step) / SDL_GetPerformanceFrequency();
  as->last_step = now;
  if (as->game_context->is_alive) {
    // Movement
    as->game_context->acceleration += GRAVITY * deltaTime;
    as->game_context->bird_offset += as->game_context->acceleration;
    as->last_step = SDL_GetPerformanceCounter();
    for (int i = 0; i < 2; i++) {
      as->game_context->pipes[i].x_offset -= BIRD * deltaTime * 8;
      if (as->game_context->pipes[i].x_offset + BIRD < 0) {
        as->game_context->pipes[i].x_offset += WIDTH + BIRD;
        as->game_context->pipes[i].y_offset =
            (HEIGHT / 2.0) + (SDL_rand(HEIGHT / 3.0));
      }
    }
    // Collision
    if (check_collisions(as->game_context)) {
      as->game_context->is_alive = false;
    }
    // Drawing
    draw_game_context(as->renderer, as->game_context);
  } else {
    SDL_SetRenderDrawColor(as->renderer, 26, 26, 26, 1);
    SDL_RenderClear(as->renderer);
    SDL_RenderPresent(as->renderer);
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
