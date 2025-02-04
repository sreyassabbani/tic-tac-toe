#include <SDL.h>
#include <stdio.h>

#define SCREEN_WIDTH_PX 400
#define SCREEN_HEIGHT_PX 600
#define SCREEN_WIDTH 3
#define SCREEN_HEIGHT 3
#define SCREEN_PADDING_LEFT_PX 50
#define SCREEN_PADDING_RIGHT_PX 50
#define SCREEN_PADDING_TOP_PX 50
#define SCREEN_PADDING_BOTTOM_PX 90

#define BLOCK_WIDTH_PX 100
#define BLOCK_HEIGHT_PX 100

#define GRAY 230, 230, 230
#define WHITE 255, 255, 255


void rerender(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, WHITE, 255);
  SDL_RenderClear(renderer);
}

void render_random_block(SDL_Renderer* renderer) {
}

void render_block(SDL_Renderer* renderer, SDL_Point* position) {
  SDL_SetRenderDrawColor(renderer, GRAY, 255);
  
  // could just take in SDL_Rect instead of taking up more memory...
  SDL_Rect rect = {
    .x = position->x,
    .y = position->y,
    .w = BLOCK_WIDTH_PX - 10,
    .h = BLOCK_HEIGHT_PX - 10,
  };

  SDL_RenderFillRect(renderer, &rect);
}

void draw_circle(SDL_Renderer* renderer, SDL_Point* position) {
  
}

void draw_cross(SDL_Renderer* renderer, SDL_Point* position) {
  
}

void draw_gameboard(SDL_Renderer* renderer, int8_t* board) {
  // row-major order
  SDL_Point squares[9];
  int i = 0;
  for (int col = 0; col < 3; ++col) {
    for (int row = 0; row < 3; ++row) {
      squares[i] = (SDL_Point){
        .x = SCREEN_PADDING_LEFT_PX + col * BLOCK_WIDTH_PX + 5,
        .y = SCREEN_PADDING_TOP_PX + row * BLOCK_HEIGHT_PX + 5,
      };
      render_block(renderer, &squares[i]);
      ++i;
    }
  }
}


int main() {
  SDL_Window* window;
  SDL_Renderer* renderer;
  window = SDL_CreateWindow("Tic Tac Toe", 0, 0, SCREEN_WIDTH_PX, SCREEN_HEIGHT_PX, SDL_WINDOW_HIDDEN);
  SDL_ShowWindow(window);

  renderer = SDL_CreateRenderer(
    window,
    -1,
    SDL_RENDERER_PRESENTVSYNC
  );
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  printf("works");
  fflush(stdout);

  SDL_Event event;
  int running = 1;
  int init = 1;

  // All values are zero
  int8_t board[9] = { 0 };

  while (running) {
    // Redraw background for next frame
    SDL_SetRenderDrawColor(renderer, WHITE, 255);
    SDL_RenderClear(renderer);

    // Redraw gameboard
    draw_gameboard(renderer, board);

    // Present the rendered frame
    SDL_RenderPresent(renderer);

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = 0;
      }
      
      if (event.type == SDL_MOUSEBUTTONDOWN) {
        switch (event.button.button) {
          case SDL_BUTTON_LEFT: {
            SDL_Point pt = { 50, 50 };
            // is this really fast? could probably just send in int by value...
            rerender(renderer);
            render_block(renderer, &pt);
            SDL_RenderPresent(renderer);
            break;
          }
        }
      }
    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

