#include "utils.h"


SDL_Window* GetGlobalWindow() {
  static SDL_Window* window = SDL_CreateWindow("SDL3 Event Loop", 800, 600, SDL_WINDOW_VULKAN);

  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  return window;
}


SDL_Renderer* GetGlobalRenderer() {
  static SDL_Renderer* renderer = SDL_CreateRenderer(GetGlobalWindow(), nullptr);
  return renderer;
}