/*
 * @Author: lenomirei lenomirei@163.com
 * @Date: 2025-09-22 11:14:24
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2026-04-07 16:28:51
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

BrowserWindow::BrowserWindow(const std::string& window_name, uint32_t window_id, CefRefPtr<CefBrowser> browser, Observer* observer, bool show)
    : name_(window_name),
      window_id_(window_id),
      browser_(browser),
      observer_(observer),
      show_(show) {
  address_ = "https://www.bing.com";
}

BrowserWindow::~BrowserWindow() {
  // demo_cef_client_ = nullptr;
}

void BrowserWindow::CreateBrowser(CefRefPtr<CefClient> client) {
  CefPostTask(CefThreadId::TID_UI, base::BindOnce([](CefRefPtr<CefClient> client, std::string address) {
    CefWindowInfo window_info;
    window_info.windowless_rendering_enabled = 1;
    CefBrowserSettings browser_settings;
    browser_settings.windowless_frame_rate = 30;
    CefBrowserHost::CreateBrowser(window_info, client, address, browser_settings, nullptr, nullptr);
  }, client, "https://www.bing.com"));
}

void BrowserWindow::Show() {
  show_ = true;
}

void BrowserWindow::Close() {
  if (browser_) {
    CefPostTask(CefThreadId::TID_UI,
                base::BindOnce(
                    [](CefRefPtr<CefBrowser> browser) {
                      browser->GetHost()->CloseBrowser(false);
                    },
                    browser_));
  } else {
    show_ = false;
    if (observer_) {
      observer_->OnBrowserWindowClosed(window_id_);
    } 
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
      CefPostTask(CefThreadId::TID_UI, base::BindOnce([](CefRefPtr<CefBrowser> browser, std::string address) {
        browser->GetMainFrame()->LoadURL(address);
      }, browser_, address_));
    }

    ImVec2 avail = ImGui::GetContentRegionAvail();

    if (tex_ != nullptr && (avail.x != browser_available_width_ || avail.y != browser_available_height_)) {
      bool hidden = avail.x <= 0 || avail.y <= 0;
      if (browser_hide_ != hidden) {
        browser_hide_ = hidden;
        HandleBrowserHidden();
      } else {
        if (debounce_timer_id_ != 0) {
          SDL_RemoveTimer(debounce_timer_id_);
        }

        debounce_timer_id_ = SDL_AddTimer(500, [](void* user_data, SDL_TimerID timer_id, uint32_t interval) -> uint32_t {
        // this will be called in timer thread
        auto browser_window = (BrowserWindow*)user_data;
        browser_window->OnDebounceTimerCallback();
        return 0; }, this);
      }
    }
    browser_available_width_ = avail.x;
    browser_available_height_ = avail.y;
    ImGui::BeginChild("browser area", avail, false);

    if (tex_ != nullptr) {
      std::lock_guard<std::mutex> lock(mutex_);
      if (image_buffer_ != nullptr) {
        SDL_UpdateTexture(tex_, NULL, image_buffer_, tex_->w * 4);
        ImGui::InvisibleButton("browser_invisible_button", ImVec2(tex_->w, tex_->h));
        ImVec2 pos = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        ImGui::GetWindowDrawList()->AddImage((ImTextureID)tex_, pos, max);
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
  tex_ = SDL_CreateTexture(GetGlobalRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, browser_buffer_width_, browser_buffer_height_);

  std::lock_guard<std::mutex> lock(mutex_);
  if (image_buffer_ == nullptr) {
    std::cout << "create image buffer " << browser_buffer_width_ << " " << browser_buffer_width_ << std::endl;
    image_buffer_ = new unsigned char[browser_buffer_width_ * browser_buffer_height_ * 4];
    memset(image_buffer_, 0, browser_buffer_width_ * browser_buffer_height_ * 4);
  }
}

void BrowserWindow::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) {
  if (browser_buffer_width_ != width || browser_buffer_height_ != height) {
    browser_buffer_width_ = width;
    browser_buffer_height_ = height;

    SDL_RunOnMainThread([](void* user_data) {
      auto browser_window = (BrowserWindow*)user_data;
      browser_window->RecreateTexture();
    }, this, true);
  }
  {
    std::lock_guard<std::mutex> lock(mutex_);
    if (image_buffer_ != nullptr)
      memcpy(image_buffer_, buffer, browser_buffer_width_ * browser_buffer_height_ * 4);
  }
}

void BrowserWindow::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
  // in browser thread risk?
  rect.x = 0;
  rect.y = 0;
  rect.width = browser_available_width_;
  rect.height = browser_available_height_;
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
  browser_ = nullptr;
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
      CefPostTask(CefThreadId::TID_UI, base::BindOnce([](CefRefPtr<CefBrowser> browser, CefMouseEvent mouse_event) {
                    if (browser && browser->GetHost()) {
                      browser->GetHost()->SendMouseClickEvent(mouse_event, CefBrowserHost::MouseButtonType::MBT_LEFT, false, 1);
                    }
                  }, browser_, mouse_event));
    }
    mouse_down = true;
    mouse_event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
  }
  CefPostTask(CefThreadId::TID_UI, base::BindOnce([](CefRefPtr<CefBrowser> browser, CefMouseEvent mouse_event) {
                if (browser && browser->GetHost()) {
                  browser->GetHost()->SendMouseMoveEvent(mouse_event, (mouse_event.x < 0 || mouse_event.y < 0) ? true : false);
                }
              },
              browser_, mouse_event));

  if (ImGui::IsMouseReleased(0)) {
    CefPostTask(CefThreadId::TID_UI, base::BindOnce([](CefRefPtr<CefBrowser> browser, CefMouseEvent mouse_event) {
                  if (browser && browser->GetHost()) {
                    browser->GetHost()->SendMouseClickEvent(mouse_event, CefBrowserHost::MouseButtonType::MBT_LEFT, true, 1);
                  }
                }, browser_, mouse_event));
    mouse_down = false;
  }

}

void BrowserWindow::OnDebounceTimerCallback() {
  std::lock_guard<std::mutex> lock(mutex_);
  delete[] image_buffer_;
  image_buffer_ = nullptr;

  if (!browser_hide_) {
    CefPostTask(CefThreadId::TID_UI, base::BindOnce([](CefRefPtr<CefBrowser> browser) {
                  if (browser && browser->GetHost()) {
                    browser->GetHost()->WasResized();
                  }
                },
                                                    browser_));
  }
}

void BrowserWindow::HandleBrowserHidden() {
  CefPostTask(CefThreadId::TID_UI, base::BindOnce([](CefRefPtr<CefBrowser> browser, bool hidden) {
                if (browser && browser->GetHost()) {
                      browser->GetHost()->WasHidden(hidden);

                }
              }, browser_, browser_hide_));
}