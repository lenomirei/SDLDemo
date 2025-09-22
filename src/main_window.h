/*
 * @Author: lenomirei lenomirei@163.com
 * @Date: 2025-09-22 11:14:12
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2025-09-22 15:41:56
 * @FilePath: \SDLDemo\src\main_window.h
 * @Description:
 *
 */

#include "cef/include/cef_base.h"
#include "demo_cef_client.h"
#include "SDL3/SDL.h"
#include <mutex>

class MainWindow : public CefBaseRefCounted, public DemoCefClient::Delegate {
 public:
  MainWindow();
  void CreateBrowser();
  void Draw();

  virtual void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height);
  virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect);
  virtual bool OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info);
  virtual void GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info);
  virtual void CanClose();

 protected:
  CefRefPtr<DemoCefClient> demo_cef_client_ = nullptr;
  int width_ = 400;
  int height_ = 400;
  SDL_Texture* tex_ = nullptr;
  std::mutex mutex_;
  unsigned char* image_buffer_ = nullptr;

 private:
  IMPLEMENT_REFCOUNTING(MainWindow);
};