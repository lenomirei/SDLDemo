/*
 * @Author:
 * @Date: 2025-09-19 18:45:32
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2026-04-07 10:28:08
 * @FilePath: \SDLDemo\src\demo_cef_client.h
 * @Description:
 *
 */
#include "cef/include/cef_client.h"
#include "cef/include/cef_render_handler.h"

class DemoCefClient : public CefClient, public CefRenderHandler, public CefLifeSpanHandler, public CefDisplayHandler, public CefLoadHandler {
 public:
  class Delegate {
   public:
    virtual void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) = 0;
    virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) = 0;
    virtual bool OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info) = 0;
    virtual void GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) = 0;
    virtual void CanClose(CefRefPtr<CefBrowser> browser) = 0;
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) = 0;
  };
  DemoCefClient(Delegate* delegate);

 protected:
  // override from CefClient
  CefRefPtr<CefRenderHandler> GetRenderHandler() override;
  CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
  CefRefPtr<CefDisplayHandler> GetDisplayHandler() override;
  CefRefPtr<CefLoadHandler> GetLoadHandler() override;

  // override from CefRenderHandler
  virtual void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) override;
  virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
  virtual bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) override;

  // override from CefDisplayHandler
  virtual bool OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info) override;

  // override from CefLifeSpanHandler
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

  // override from CefLoadHandler
  virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) override;

 private:
  Delegate* delegate_ = nullptr;

 private:
  IMPLEMENT_REFCOUNTING(DemoCefClient);
};