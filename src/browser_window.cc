/*
 * @Author: lenomirei lenomirei@163.com
 * @Date: 2025-09-22 11:14:24
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2025-10-13 11:33:04
 * @FilePath: \SDLDemo\src\browser_window.cc
 * @Description:
 *
 */
#include "browser_window.h"

#include <iostream>
#include <string>
#include "cef/include/base/cef_callback.h"
#include "cef/include/wrapper/cef_closure_task.h"
#include "imgui/backends/imgui_impl_sdl3.h"
#include "imgui/backends/imgui_impl_sdlrenderer3.h"
#include "imgui/imgui_stdlib.h"
#include "utils.h"

BrowserWindow::BrowserWindow(const std::string& window_name, bool show)
    : name_(window_name),
      show_(show) {
  address_ = "https://www.google.com";
}

void BrowserWindow::CreateBrowser() {
  demo_cef_client_ = new DemoCefClient(this);
  CefPostTask(CefThreadId::TID_UI, base::BindOnce([](CefRefPtr<DemoCefClient> client, std::string address) {
    CefWindowInfo window_info;
    window_info.windowless_rendering_enabled = 1;
    CefBrowserSettings browser_settings;
    browser_settings.windowless_frame_rate = 30;
    CefBrowserHost::CreateBrowser(window_info, client, address, browser_settings, nullptr, nullptr);
  }, demo_cef_client_, address_));
  
}

void BrowserWindow::Show() {
  show_ = true;
}

void BrowserWindow::Close() {
  if (demo_cef_client_ != nullptr) {
    demo_cef_client_->CloseBrowser();
  } else {
    show_ = false;
  }
}

void BrowserWindow::Draw() {
  if (!show_) {
    return;
  }

  // begin a imgui window
  {
    ImGui::Begin(name_.c_str(), nullptr, 0);

    if (ImGui::Button("Close")) {
      Close();
    }

    ImGui::SameLine();
    ImGui::InputText("address", &address_, 0);
    ImGui::SameLine();
    if (ImGui::Button("Go!")) {
      CefPostTask(CefThreadId::TID_UI, base::BindOnce([](CefRefPtr<DemoCefClient> client, std::string address) {
        client->GetBrowser()->GetMainFrame()->LoadURL(address);
      }, demo_cef_client_, address_));
    }

    if (ImGui::Button("Create Browser")) {
      CreateBrowser();
    }

    ImVec2 avail = ImGui::GetContentRegionAvail();

    if (tex_ != nullptr && (avail.x != width_ || avail.y != height_)) {
      if (debounce_timer_id_ != 0) {
        SDL_RemoveTimer(debounce_timer_id_);
      }

      debounce_timer_id_ = SDL_AddTimer(500, [](void* user_data, SDL_TimerID timer_id, uint32_t interval) -> uint32_t {
        // this will be called in timer thread
        auto browser_window = (BrowserWindow*)user_data;
        browser_window->OnDebounceTimerCallback();
        return 0;
      }, this);
    }
    width_ = avail.x;
    height_ = avail.y;
    ImGui::BeginChild("browser area", avail, false);

    if (tex_ != nullptr) {
      std::lock_guard<std::mutex> lock(mutex_);
      if (image_buffer_ != nullptr) {
        SDL_UpdateTexture(tex_, NULL, image_buffer_, tex_->w * 4);
        ImGui::Image((ImTextureID)tex_, ImVec2(tex_->w, tex_->h));
      }
    }
    ImGui::SetMouseCursor(cursor_type_);
    if (ImGui::IsItemHovered()) {
      ImGui::GetIO().WantCaptureMouse = false;
      HandleBrowserEvent();
    }
    ImGui::EndChild();

    ImGui::End();
  }

}

void BrowserWindow::RecreateTexture() {
  // in main thread
  if (tex_ != nullptr) {
    SDL_DestroyTexture(tex_);
    tex_ = nullptr;
  }
  tex_ = SDL_CreateTexture(GetGlobalRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, browser_width_, browser_height_);

  std::lock_guard<std::mutex> lock(mutex_);
  if (image_buffer_ == nullptr) {
    image_buffer_ = new unsigned char[browser_width_ * browser_height_ * 4];
    memset(image_buffer_, 0, browser_width_ * browser_height_ * 4);
  }
}

void BrowserWindow::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) {
  if (browser_width_ != width || browser_height_ != height) {
    browser_width_ = width;
    browser_height_ = height;

    SDL_RunOnMainThread([](void* user_data) {
      auto browser_window = (BrowserWindow*)user_data;
      browser_window->RecreateTexture();
    }, this, true);
  }
  {
    std::lock_guard<std::mutex> lock(mutex_);
    if (image_buffer_ != nullptr)
      memcpy(image_buffer_, buffer, browser_width_ * browser_height_ * 4);
  }
}

void BrowserWindow::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
  // in browser thread risk?
  rect.x = 0;
  rect.y = 0;
  rect.width = width_;
  rect.height = height_;
}

bool BrowserWindow::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info) {
  cursor_type_ = ImGuiMouseCursor_::ImGuiMouseCursor_Arrow;
  switch (type) {
    case CT_POINTER:
      cursor_type_ = ImGuiMouseCursor_::ImGuiMouseCursor_Arrow;
      break;
    case CT_HAND:
      cursor_type_ = ImGuiMouseCursor_::ImGuiMouseCursor_Hand;
      break;
    case CT_IBEAM:
      cursor_type_ = ImGuiMouseCursor_::ImGuiMouseCursor_TextInput;
      break;
    default:
      cursor_type_ = ImGuiMouseCursor_::ImGuiMouseCursor_Arrow;
      break;
  }

  return true;
}

void BrowserWindow::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) {
  screen_info.device_scale_factor = 1.0;
}

void BrowserWindow::CanClose() {
  demo_cef_client_ = nullptr;
  Close();
}

void BrowserWindow::HandleBrowserEvent() {
  auto io = ImGui::GetIO();
  static bool mouse_down = false;
  ImVec2 browser_pos = ImGui::GetItemRectMin();
  CefMouseEvent mouse_event;
  mouse_event.x = io.MousePos.x - browser_pos.x;
  mouse_event.y = io.MousePos.y - browser_pos.y;

  if (ImGui::IsMouseDown(0)) {
    if (!mouse_down) {
      CefPostTask(CefThreadId::TID_UI, base::BindOnce([](CefRefPtr<DemoCefClient> client, CefMouseEvent mouse_event) {
                    if (client && client->GetBrowser() && client->GetBrowser()->GetHost()) {
                      client->GetBrowser()->GetHost()->SendMouseClickEvent(mouse_event, CefBrowserHost::MouseButtonType::MBT_LEFT, false, 1);
                    }
                  }, demo_cef_client_, mouse_event));
    }
    mouse_down = true;
    mouse_event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
  }
  CefPostTask(CefThreadId::TID_UI, base::BindOnce([](CefRefPtr<DemoCefClient> client, CefMouseEvent mouse_event) {
                if (client && client->GetBrowser() && client->GetBrowser()->GetHost()) {
                  client->GetBrowser()->GetHost()->SendMouseMoveEvent(mouse_event, (mouse_event.x < 0 || mouse_event.y < 0) ? true : false);
                }
              },
              demo_cef_client_, mouse_event));

  if (ImGui::IsMouseReleased(0)) {
    CefPostTask(CefThreadId::TID_UI, base::BindOnce([](CefRefPtr<DemoCefClient> client, CefMouseEvent mouse_event) {
                  if (client && client->GetBrowser() && client->GetBrowser()->GetHost()) {
                    client->GetBrowser()->GetHost()->SendMouseClickEvent(mouse_event, CefBrowserHost::MouseButtonType::MBT_LEFT, true, 1);
                  }
                }, demo_cef_client_, mouse_event));
    mouse_down = false;
  }

}

void BrowserWindow::OnDebounceTimerCallback() {
  std::lock_guard<std::mutex> lock(mutex_);
  delete[] image_buffer_;
  image_buffer_ = nullptr;
  browser_height_ = 0;
  browser_width_ = 0;

  bool hidden = false;
  if (height_ <= 0 || width_ <= 0) {
    hidden = true;
  }

  CefPostTask(CefThreadId::TID_UI, base::BindOnce([](CefRefPtr<DemoCefClient> client, bool hidden) {
                if (client != nullptr && client->GetBrowser() != nullptr && client->GetBrowser()->GetHost() != nullptr) {
                  if (hidden) {
                    client->GetBrowser()->GetHost()->WasHidden(true);
                  } else {
                    client->GetBrowser()->GetHost()->WasHidden(false);
                    client->GetBrowser()->GetHost()->WasResized();
                  }
                }
              }, demo_cef_client_, hidden));
}