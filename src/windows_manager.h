/*
 * @Author: lenomirei lenomirei@163.com
 * @Date: 2025-12-18 15:08:10
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2026-04-07 10:04:52
 * @FilePath: \SDLDemo\src\windows_manager.h
 * @Description: 
 */
#include "browser_window.h"

#include "demo_cef_client.h"
#include <unordered_map>

class WindowsManager : public BrowserWindow::Observer, public DemoCefClient::Delegate {
 public:
  WindowsManager() = default;
  ~WindowsManager() = default;

  void Initialize();
  void CreateNewWindow(const std::string& name = "new window",
                       bool show = true);

  void RemoveWindow(uint32_t id);

  CefRefPtr<BrowserWindow> GetWindow(uint32_t id) {
    auto it = windows_map_.find(id);
    if (it != windows_map_.end()) {
      return it->second;
    }
    return nullptr;
  }

  void RenderAllWindows();

  void CloseAllWindows();

  virtual void OnBrowserWindowClosed(const uint32_t window_id) override;

  virtual void OnPaint(CefRefPtr<CefBrowser> browser,
                       CefRenderHandler::PaintElementType type,
                       const CefRenderHandler::RectList& dirtyRects,
                       const void* buffer,
                       int width,
                       int height) override;
  virtual void GetViewRect(CefRefPtr<CefBrowser> browser,
                           CefRect& rect) override;
  virtual bool OnCursorChange(CefRefPtr<CefBrowser> browser,
                              CefCursorHandle cursor,
                              cef_cursor_type_t type,
                              const CefCursorInfo& custom_cursor_info) override;
  virtual void GetScreenInfo(CefRefPtr<CefBrowser> browser,
                             CefScreenInfo& screen_info) override;
  virtual void CanClose(CefRefPtr<CefBrowser> browser) override;
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;

 protected:
  std::unordered_map<uint32_t, CefRefPtr<BrowserWindow>> windows_map_;
  std::vector<uint32_t> pending_removal_window_ids_;
  CefRefPtr<DemoCefClient> client_ = nullptr;
};