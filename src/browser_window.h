/*
 * @Author: lenomirei lenomirei@163.com
 * @Date: 2025-09-22 11:14:12
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2026-04-07 16:30:01
 * @FilePath: \SDLDemo\src\main_window.h
 * @Description:
 *
 */

#include "SDL3/SDL.h"
#include "cef/include/cef_base.h"
#include "cef/include/cef_client.h"
#include "imgui/imgui.h"

#include <mutex>

class BrowserWindow : public CefBaseRefCounted {
 public:
  class Observer {
   public:
    virtual void OnBrowserWindowClosed(const uint32_t window_id) = 0;
  };
  BrowserWindow(const std::string& window_name,
                uint32_t window_id,
                CefRefPtr<CefBrowser> browser,
                Observer* observer = nullptr,
                bool show = true);
  ~BrowserWindow();
  static void CreateBrowser(CefRefPtr<CefClient> client);
  void Draw();
  void Show();
  void Close();

  void OnPaint(CefRefPtr<CefBrowser> browser,
                       CefRenderHandler::PaintElementType type,
                       const CefRenderHandler::RectList& dirtyRects,
                       const void* buffer,
                       int width,
                       int height);
  void GetViewRect(CefRefPtr<CefBrowser> browser,
                           CefRect& rect);
  bool OnCursorChange(CefRefPtr<CefBrowser> browser,
                              CefCursorHandle cursor,
                              cef_cursor_type_t type,
                              const CefCursorInfo& custom_cursor_info);
  void GetScreenInfo(CefRefPtr<CefBrowser> browser,
                             CefScreenInfo& screen_info);
  void CanClose();

  void OnDebounceTimerCallback();
  void RecreateTexture();

 protected:
  void HandleBrowserHidden();
  void HandleBrowserEvent();

 protected:
  CefRefPtr<CefBrowser> browser_ = nullptr;
  int browser_available_width_ = 1;
  int browser_available_height_ = 1;
  int browser_buffer_width_ = 1;
  int browser_buffer_height_ = 1;
  SDL_Texture* tex_ = nullptr;
  std::mutex mutex_;
  unsigned char* image_buffer_ = nullptr;
  ImGuiMouseCursor cursor_type_ = ImGuiMouseCursor_::ImGuiMouseCursor_Arrow;
  std::string address_;
  SDL_TimerID debounce_timer_id_ = 0;

  std::string name_;
  uint32_t window_id_;
  bool show_ = false;
  bool browser_hide_ = false;
  Observer* observer_ = nullptr;

 private:
  IMPLEMENT_REFCOUNTING(BrowserWindow);
};