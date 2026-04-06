#include "SDL3/SDL.h"

#include <iostream>
#include "imgui/backends/imgui_impl_sdl3.h"
#include "imgui/backends/imgui_impl_sdlrenderer3.h"
#include "imgui/imgui.h"
#include "demo_cef_app.h"
#include "windows_manager.h"
#include "utils.h"

int main() {
  CefEnableHighDPISupport();
  CefMainArgs main_args(GetModuleHandle(0));

  int exit_code = CefExecuteProcess(main_args, nullptr, nullptr);
  if (exit_code >= 0) {
    // The sub-process has completed so return here.
    return exit_code;
  }

  CefSettings settings;
  settings.multi_threaded_message_loop = 1;
  settings.windowless_rendering_enabled = 1;
  CefRefPtr<CefApp> app = new DemoCefApp();
  CefInitialize(main_args, settings, app.get(), nullptr);


  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << '\n';
    return 1;
  }

  SDL_Window* window = GetGlobalWindow();
  if (window == nullptr) {
    std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << '\n';
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* renderer = GetGlobalRenderer();
  if (renderer == nullptr) {
    std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << '\n';
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Setup imgui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsClassic();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);

  bool running = true;
  SDL_Event event;

  auto texture = SDL_CreateTexture(renderer, SDL_PixelFormat::SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, 800, 600);

  std::shared_ptr<WindowsManager> windows_manager = std::make_shared<WindowsManager>();

  const int frame_time = 1000 / 25; // 25 FPS
  auto last_time = SDL_GetTicks();
  
  while (running) {
    auto now = SDL_GetTicks();
    auto delta = now - last_time;
    if (delta < frame_time) {
        SDL_Delay((Uint32)(frame_time - delta)); // delay
        continue;
    }
    last_time = now;

    // Handle SDL event
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL3_ProcessEvent(&event);

      if (event.type == SDL_EVENT_QUIT) {
        running = false;
      }
    }

    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("manager window");
    if (ImGui::Button("new window")) {
      windows_manager->CreateNewWindow();
    }

    windows_manager->RenderAllWindows();
    ImGui::End();

    ImGui::Render();

    // clear!
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
  }

  windows_manager->CloseAllWindows();

  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  CefShutdown();
  return 0;
}