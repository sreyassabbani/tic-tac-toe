#include <SDL.h>
#include <SDL_ttf.h>

#include <stdint.h>
#include <stdbool.h>
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

#define RED 255, 0, 0
#define BLUE 0, 0, 255
#define GRAY 230, 230, 230
#define WHITE 255, 255, 255

#define FONT "/Library/Fonts/Arial Unicode.ttf"

// Players, "x" and "o"
typedef enum Player
{
  Blue = 1,
  Red = 2
} Player;

// Record board state
// note that `Playing` is falsy
typedef enum
{
  Playing = 0,
  BlueWon = 1,
  RedWon = 2,
  Draw = 3
} BoardState;

typedef struct
{
  BoardState state;
  uint8_t board[9];
  Player turn;
  uint8_t moves_left;
} Game;

void rerender_background(SDL_Renderer *renderer, uint8_t r, uint8_t g, uint8_t b)
{
  SDL_SetRenderDrawColor(renderer, r, g, b, 255);
  SDL_RenderClear(renderer);
}

void render_random_block(SDL_Renderer *renderer)
{
}

void render_block(SDL_Renderer *renderer, SDL_Point *position, uint8_t r, uint8_t g, uint8_t b)
{
  SDL_SetRenderDrawColor(renderer, r, g, b, 255);

  // could just take in SDL_Rect instead of taking up more memory...
  SDL_Rect rect = {
      .x = position->x,
      .y = position->y,
      .w = BLOCK_WIDTH_PX - 10,
      .h = BLOCK_HEIGHT_PX - 10,
  };

  SDL_RenderFillRect(renderer, &rect);
}

void draw_blue(SDL_Renderer *renderer, SDL_Point *position)
{
  render_block(renderer, position, BLUE);
}

void draw_red(SDL_Renderer *renderer, SDL_Point *position)
{
  render_block(renderer, position, RED);
}

void update_game_state(Game *game, int8_t new_position)
{
  // Note: checking only the rows/columns/diagonals that have been updated allows us to omit checking for triplets of 0

  // Calculations turn out this way because `game->board` follows row-major order
  int y = new_position / 3;
  int x = new_position % 3;

  // Check vertically
  int8_t vertical_match = game->board[x];
  for (int i = 1; i < 3; ++i)
  {
    if (vertical_match != game->board[x + 3 * i])
      vertical_match = 0;
  }

  // Return if a vertical triplet is found
  if (vertical_match != 0)
  {
    game->state = (BoardState)vertical_match;
    return;
  }

  // Check horizontally
  int8_t horizontal_match = game->board[3 * y];
  for (int i = 1; i < 3; ++i)
  {
    if (horizontal_match != game->board[3 * y + i])
      horizontal_match = 0;
  }

  // Return if a horizontal triplet is found
  if (horizontal_match != 0)
  {
    game->state = (BoardState)horizontal_match;
    return;
  }

  // Check diagonally (0-indexed row-major `new_position` modulo 2 gives the pattern we are looking for; draw it)
  // Also equivalent to binary operation `new_position & 1`
  if (new_position % 2 == 0) {
    int8_t diagonal_match = game->board[2];
    
    // Top-right to bottom-left
    if (new_position % 2 == 0 && new_position % 8 != 0) {
      for (int i = 4; i <= 3 * 2; i += 2) {
        if (diagonal_match != game->board[i]) diagonal_match = 0;
      }
      if (diagonal_match != 0) {
        game->state = (BoardState) diagonal_match;
        return;
      }
    }
    
    // Top-left to bottom-right
    if (new_position % 4 == 0) {
      diagonal_match = game->board[0];
      for (int i = 4; i < 3 * 4; i += 4) {
        if (diagonal_match != game->board[i]) diagonal_match = 0;    
      }
      if (diagonal_match != 0) {
        game->state = (BoardState) diagonal_match;
        return;
      }
    }
  }

  // If moves left is 0 and no match is found, set state to `Draw`
  if (game->moves_left == 0)
    game->state = Draw;

  // No triplets found, BoardState remains `Playing`
}

void draw_gameboard(SDL_Renderer *renderer, uint8_t *board)
{
  // row-major order
  SDL_Point squares[9];
  int i = 0;
  for (int row = 0; row < 3; ++row)
  {
    for (int col = 0; col < 3; ++col)
    {
      squares[i] = (SDL_Point){
          .x = SCREEN_PADDING_LEFT_PX + col * BLOCK_WIDTH_PX + 5,
          .y = SCREEN_PADDING_TOP_PX + row * BLOCK_HEIGHT_PX + 5,
      };
      switch (board[i])
      {
      case 0:
        render_block(renderer, &squares[i], GRAY);
        break;
      case 1:
        draw_blue(renderer, &squares[i]);
        break;
      case 2:
        draw_red(renderer, &squares[i]);
        break;
      }
      ++i;
    }
  }
}

int8_t position_on_board(int mouse_x, int mouse_y)
{
  // whyyyyy
  mouse_y -= 74;
  if (mouse_x < 50 || mouse_x > 50 + 300)
    return -1;
  if (mouse_y < 50 || mouse_y > 50 + 300)
    return -1;

  return ((mouse_x - SCREEN_PADDING_LEFT_PX) / BLOCK_WIDTH_PX) + 3 * ((mouse_y - SCREEN_PADDING_TOP_PX) / BLOCK_HEIGHT_PX);
}

bool move(SDL_Renderer *renderer, uint8_t *board, uint8_t *moves_left, Player player, uint8_t position)
{
  if (board[position] == 0)
  {
    board[position] = player;
    --*moves_left;
    return true;
  }
  return false;
}

void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Point point)
{
  int w = 0;
  int h = 0;
  int padding = 8;

  TTF_SizeText(font, text, &w, &h);

  SDL_Color font_color = {.r = 255, .g = 255, .b = 255, .a = 255};
  SDL_Surface *surface = TTF_RenderText_Solid(font, text, font_color);

  SDL_Rect rect = {
      .x = point.x - (w / 2),
      .y = point.y - (h / 2),
      .w = w,
      .h = h,
  };

  SDL_Rect text_background = {
      .x = rect.x - padding,
      .y = rect.y - padding,
      .w = rect.w + padding * 2,
      .h = rect.h + padding * 2};

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderFillRect(renderer, &text_background);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderDrawRect(renderer, &text_background);

  // render text
  SDL_RenderCopy(renderer, texture, NULL, &rect);

  // clean
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void render_caption(SDL_Renderer *renderer, TTF_Font *font, const char *text)
{
  render_text(renderer, font, text, (SDL_Point){.x = 200, .y = 500});
}

int main()
{
  SDL_Window *window;
  SDL_Renderer *renderer;
  window = SDL_CreateWindow("Tic Tac Toe", 0, 0, SCREEN_WIDTH_PX, SCREEN_HEIGHT_PX, SDL_WINDOW_HIDDEN);
  SDL_ShowWindow(window);

  renderer = SDL_CreateRenderer(
      window,
      -1,
      SDL_RENDERER_ACCELERATED);

  TTF_Init();
  TTF_Font *font = TTF_OpenFont(FONT, 20);

  SDL_Event event;
  int running = 1;
  int init = 1;
  int mouse_x, mouse_y;

  Game game = {
      .state = Playing,
      .turn = Red,
      .board = {0},
      .moves_left = 9,
  };

  while (running)
  {

    // redraw background for next frame
    SDL_SetRenderDrawColor(renderer, WHITE, 255);
    SDL_RenderClear(renderer);

    // redraw gameboard
    draw_gameboard(renderer, game.board);

    switch (game.state)
    {
    case Playing:
      // display turn
      switch (game.turn)
      {
      case Red:
        render_caption(renderer, font, "Player Red's turn!");
        break;
      case Blue:
        render_caption(renderer, font, "Player Blue's turn!");
        break;
      }
      break;
    case BlueWon:
      render_caption(renderer, font, "Blue Won!");
      break;
    case RedWon:
      render_caption(renderer, font, "Red won!");
      break;
    case Draw:
      render_caption(renderer, font, "Game drawn!");
      break;
    }

    // present the rendered frame
    SDL_RenderPresent(renderer);

    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
      {
        running = 0;
      }

      if (event.type == SDL_MOUSEBUTTONDOWN)
      {
        // Ignore input if game is over
        if (game.state != Playing)
          continue;

        SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
        int8_t i = position_on_board(mouse_x, mouse_y);

        if (i == -1)
          continue;

        switch (event.button.button)
        {
        case SDL_BUTTON_LEFT:
          if (game.turn == Red)
          {
            if (move(renderer, game.board, &game.moves_left, Red, i))
            {
              update_game_state(&game, i);
              game.turn = Blue;
              printf("Blue turn\n");
              fflush(stdout);
            }
          }
          break;
        case SDL_BUTTON_RIGHT:
          if (game.turn == Blue)
          {
            if (move(renderer, game.board, &game.moves_left, Blue, i))
            {
              update_game_state(&game, i);
              game.turn = Red;
              printf("Red turn\n");
              fflush(stdout);
            }
          }
          break;
        }
      }
    }
  }

  TTF_CloseFont(font);
  TTF_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
