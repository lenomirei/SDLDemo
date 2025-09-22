/*
 * @Author:
 * @Date: 2025-09-19 18:45:32
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2025-09-19 19:23:40
 * @FilePath: \SDLDemo\src\demo_cef_client.h
 * @Description:
 *
 */
#include "cef/include/cef_client.h"
#include "cef/include/cef_render_handler.h"

class DemoCefClient : public CefClient, public CefRenderHandler, public CefLifeSpanHandler, public CefDisplayHandler {
 public:
  class Delegate {
   public:
    virtual void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) = 0;
    virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) = 0;
    virtual bool OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info) = 0;
    virtual void GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) = 0;
    virtual void CanClose() = 0;
  };
  DemoCefClient(Delegate* delegate);

  CefRefPtr<CefBrowser> GetBrowser();
  void CloseBrowser();

 protected:
  CefRefPtr<CefRenderHandler> GetRenderHandler() override;
  CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
  CefRefPtr<CefDisplayHandler> GetDisplayHandler() override;

  // override from CefRenderHandler
  void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) override;
  void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
  bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) override;

  // override from CefDisplayHandler
  bool OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info) override;

  // override from CefLifeSpanHandler
  bool DoClose(CefRefPtr<CefBrowser> browser) override;
  void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

 private:
  Delegate* delegate_ = nullptr;
  CefRefPtr<CefBrowser> browser_;

 private:
  IMPLEMENT_REFCOUNTING(DemoCefClient);
};