/*
 * @Author: lenomirei lenomirei@163.com
 * @Date: 2025-09-22 11:14:12
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2025-10-10 15:53:01
 * @FilePath: \SDLDemo\src\main_window.h
 * @Description:
 *
 */

#include "cef/include/cef_base.h"
#include "demo_cef_client.h"
#include "SDL3/SDL.h"
#include "imgui/imgui.h"

#include <mutex>


class BrowserWindow : public CefBaseRefCounted, public DemoCefClient::Delegate {
 public:
  BrowserWindow(const std::string& window_name, bool show = true);
  void CreateBrowser();
  void Draw();
  void Show();
  void Close();

  virtual void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) override;
  virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
  virtual bool OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info) override;
  virtual void GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) override;
  virtual void CanClose() override;

  void OnDebounceTimerCallback();
  void RecreateTexture();

 protected:
  void HandleBrowserEvent(); 

 protected:
  CefRefPtr<DemoCefClient> demo_cef_client_ = nullptr;
  int width_ = 0;
  int height_ = 0;
  int browser_width_ = 0;
  int browser_height_ = 0;
  SDL_Texture* tex_ = nullptr;
  std::mutex mutex_;
  unsigned char* image_buffer_ = nullptr;
  ImGuiMouseCursor cursor_type_ = ImGuiMouseCursor_::ImGuiMouseCursor_Arrow;
  std::string address_;
  SDL_TimerID debounce_timer_id_ = 0;

  std::string name_;
  bool show_ = false;

 private:
  IMPLEMENT_REFCOUNTING(BrowserWindow);
};