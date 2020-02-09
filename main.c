#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "stdint.h"

#include "SDL.h"
#include "SDL_image.h"


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define NUM_PREV_POS 20


typedef struct Pos
{
  uint32_t x;
  uint32_t y;
} Pos;

typedef struct State
{
  SDL_Window *window;
  SDL_Surface *screen;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  Pos positions[NUM_PREV_POS];
  uint32_t last_pos_index;
  Pos mouse_pos;
} State;


void Draw(State *state);
bool Input(State *state);
bool StateInit(State *state);
void StateDestroy(State *state);

int main(int argc, char *argv[])
{
  State state;

  memset(&state, 0, sizeof(state));

  bool quit = StateInit(&state);

  while (!quit)
  {
    quit = Input(&state);

    Draw(&state);

    SDL_Delay(30);
  }

  StateDestroy(&state);

  return 0;
}

void StateDestroy(State *state)
{
  SDL_DestroyTexture(state->texture);

  SDL_DestroyWindow(state->window);

  SDL_DestroyRenderer(state->renderer);

  IMG_Quit();
  SDL_Quit();
}

void Draw(State *state)
{
  SDL_SetRenderDrawColor(state->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(state->renderer);

  SDL_RenderCopy(state->renderer, state->texture, NULL, NULL);

  SDL_SetRenderDrawColor(state->renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);

  for (int posIndex = 0; posIndex < NUM_PREV_POS; posIndex++)
  {
    if (state->positions[posIndex].x != 0 && state->positions[posIndex].y != 0)
    {
      SDL_RenderDrawLine(state->renderer, 
                         state->positions[posIndex].x,
                         state->positions[posIndex].y,
                         state->mouse_pos.x,
                         state->mouse_pos.y);
    }
  }

  SDL_RenderPresent(state->renderer);

}

bool Input(State *state)
{
  bool quit = false;

  SDL_Event event;

  int result = SDL_PollEvent(&event);

  while (result != 0)
  {
    switch (event.type)
    {
      case SDL_QUIT:
        quit = true;
        break;

      case SDL_MOUSEMOTION:
        state->positions[state->last_pos_index] = state->mouse_pos;
        state->last_pos_index = (state->last_pos_index + 1) % NUM_PREV_POS;

        state->mouse_pos.x = event.motion.x;
        state->mouse_pos.y = event.motion.y;
        break;

      case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
          case SDLK_UP:
            printf("^\n");
            break;
          case SDLK_DOWN:
            printf("v\n");
            break;
          case SDLK_LEFT:
            printf("<\n");
            break;
          case SDLK_RIGHT:
            printf(">\n");
            break;
          case SDLK_q:
            quit = true;
            break;
        }
        break;
    }

    result = SDL_PollEvent(&event);
  }

  return quit;
}

bool StateInit(State *state)
{
  bool quit = false;

  int result = SDL_Init(SDL_INIT_VIDEO);
  if (result < 0)
  {
    printf("SDL_Init failed %d\n", result);
    quit = true;
  }

  if (!quit)
  {
    int result = SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    if (result < 0)
    {
      printf("SDL_SetHintfailed %d\n", result);
      quit = true;
    }
  }

  if (!quit)
  {
    result = IMG_Init(IMG_INIT_PNG);
    if ((result & IMG_INIT_PNG) == 0)
    {
      printf("IMG_Init returned %08X\n", result);
      quit = true;
    }
  }

  if (!quit)
  {
    state->window = SDL_CreateWindow("Test",
                                     SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED,
                                     SCREEN_WIDTH,
                                     SCREEN_HEIGHT,
                                     SDL_WINDOW_SHOWN);
    if (state->window == NULL)
    {
      printf("SDL_CreateWindow returned NULL '%s'\n", SDL_GetError());
      quit = true;
    }
  }

  if (!quit)
  {
    state->screen = SDL_GetWindowSurface(state->window);
    if (state->screen == NULL)
    {
      printf("SDL_GetWindowSurface '%s'\n", SDL_GetError());
      quit = true;
    }
  }

  if (!quit)
  {
    state->renderer = SDL_CreateRenderer(state->window, -1, SDL_RENDERER_ACCELERATED);
    if (state->renderer == NULL)
    {
      printf("SDL_CreateRenderer returned NULL '%s'\n", SDL_GetError());
      quit = true;
    }
  }

  if (!quit)
  {
    SDL_SetRenderDrawColor(state->renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    SDL_Surface *surface = IMG_Load("./assets/test.png");
    if (surface == NULL)
    {
      printf("Could not load png!\n");
      quit = true;
    }
    else
    {
      state->texture = SDL_CreateTextureFromSurface(state->renderer, surface);

      SDL_FreeSurface(surface);
    }
  }

  return quit;
}
