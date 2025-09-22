/*
 * @Author: lenomirei lenomirei@163.com
 * @Date: 2025-09-22 11:14:24
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2025-09-22 16:03:32
 * @FilePath: \SDLDemo\src\main_window.cc
 * @Description:
 *
 */
#include "main_window.h"

#include <string>
#include "imgui/backends/imgui_impl_sdl3.h"
#include "imgui/backends/imgui_impl_sdlrenderer3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "utils.h"

MainWindow::MainWindow() {

}

void MainWindow::CreateBrowser() {
  CefWindowInfo window_info;
  window_info.windowless_rendering_enabled = 1;
  CefBrowserSettings browser_settings;
  browser_settings.windowless_frame_rate = 30;
  demo_cef_client_ = new DemoCefClient(this);
  CefBrowserHost::CreateBrowser(window_info, demo_cef_client_, "https://www.baidu.com", browser_settings, nullptr, nullptr) ;
}

void MainWindow::Draw() {
  // Start the Dear ImGui frame
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  // begin a imgui window
  {
    std::string url_address;
    ImGui::Begin("Hello my first imgui program");
    ImVec2 size = ImGui::GetWindowSize();
    width_ = size.x;
    height_ = size.y;

    ImGui::InputText("address", &url_address, 0);
    if (ImGui::Button("Create Browser")) {
      CreateBrowser();
    }
    if (tex_ == nullptr) {
      tex_ = SDL_CreateTexture(GetGlobalRenderer(), SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, width_, height_);
      image_buffer_ = new unsigned char[width_ * height_ * 4];
      memset(image_buffer_, 0, width_ * height_ * 4);
    }
    {
      std::lock_guard<std::mutex> lock(mutex_);
      SDL_UpdateTexture(tex_, NULL, image_buffer_, width_ * 4);
    }

    ImGui::Image((ImTextureID)tex_, ImVec2(width_, height_));

    ImGui::End();
  }

  ImGui::Render();
}

void MainWindow::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    memcpy(image_buffer_, buffer, width * height * 4);
  }
}

void MainWindow::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
  rect.x = 0;
  rect.y = 0;
  rect.width = width_;
  rect.height = height_;
}

bool MainWindow::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info) {
  return false;
}

void MainWindow::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) {
  screen_info.device_scale_factor = 1.0;
}

void MainWindow::CanClose() {

}