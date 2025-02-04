#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH_PX 350
#define SCREEN_HEIGHT_PX 390
#define SCREEN_WIDTH 3
#define SCREEN_HEIGHT 3
#define SCREEN_PADDING_LEFT_PX 50
#define SCREEN_PADDING_RIGHT_PX 50
#define SCREEN_PADDING_TOP_PX 50
#define SCREEN_PADDING_BOTTOM_PX 90

#define BLOCK_WIDTH_PX 50
#define BLOCK_HEIGHT_PX 50

#define RED 255, 0, 0
#define WHITE 255, 255, 255


void render_block(SDL_Renderer* renderer, SDL_Point* position) {
  // could just take in SDL_Rect instead of taking up more memory...
  SDL_Rect rect = {
    .x = position->x,
    .y = position->y,
    .w = BLOCK_WIDTH_PX,
    .h = BLOCK_HEIGHT_PX,
  };

  SDL_SetRenderDrawColor(renderer, RED, 255);

  SDL_RenderFillRect(renderer, &rect);
  SDL_RenderPresent(renderer);
}

void init_gameboard(SDL_Renderer* renderer) {
  // row-major order
  SDL_Point squares[9];
  int i = 0;
  for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
      squares[i] = (SDL_Point){
        .x = SCREEN_PADDING_LEFT_PX + col * BLOCK_WIDTH_PX,
        .y = SCREEN_PADDING_TOP_PX + row * BLOCK_HEIGHT_PX,
      };
      printf("x: %d, y: %d\n", squares[i].x, squares[i].y);
      ++i;
    }
  }
  for (int sqr = 0; sqr < 9; ++sqr) {
    render_block(renderer, &squares[sqr]);
  }
}

void render_random_block(SDL_Renderer* renderer) {
}

int main() {
  SDL_Window* window;
  SDL_Renderer* renderer;
  window = SDL_CreateWindow("Tic Tac Toe", 0, 0, SCREEN_WIDTH_PX, SCREEN_HEIGHT_PX, SDL_WINDOW_HIDDEN);
  SDL_ShowWindow(window);

  renderer = SDL_CreateRenderer(
    window,
    -1,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
  );


  printf("works");
  fflush(stdout);

  init_gameboard(renderer);

  SDL_Event event;
  int running = 1;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = 0;
      }
      
  
      // if (event.type == SDL_MOUSEBUTTONDOWN) {
      //   if (event.button.button == SDL_BUTTON_RIGHT) {
      //     running = 0;
      //   }
      // }
      if (event.type == SDL_MOUSEBUTTONDOWN) {
        switch (event.button.button) {
          case SDL_BUTTON_RIGHT: {
            SDL_Point pt = { 50, 50 };
            // is this really fast? could probably just send in int by value...
            render_block(renderer, &pt);
            break;
          }
        }
      }
    }

    SDL_SetRenderDrawColor(renderer, WHITE, 255);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  // SDL_DestroyRenderer(Renderer);
  // int num;
  // scanf("%d", &num);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}
