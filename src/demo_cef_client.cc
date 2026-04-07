/*
 * @Author:
 * @Date: 2025-09-19 18:45:38
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2026-04-07 14:51:36
 * @FilePath: \SDLDemo\src\demo_cef_client.cc
 * @Description:
 *
 */
#include "demo_cef_client.h"

#include "cef/include/base/cef_callback.h"
#include "cef/include/wrapper/cef_closure_task.h"

DemoCefClient::DemoCefClient(Delegate* delegate)
    : delegate_(delegate) {
}

CefRefPtr<CefRenderHandler> DemoCefClient::GetRenderHandler() {
  return this;
}

CefRefPtr<CefLifeSpanHandler> DemoCefClient::GetLifeSpanHandler() {
  return this;
}

CefRefPtr<CefDisplayHandler> DemoCefClient::GetDisplayHandler() {
  return this;
}

CefRefPtr<CefLoadHandler> DemoCefClient::GetLoadHandler() {
  return this;
}

void DemoCefClient::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) {
  if (delegate_)
    delegate_->OnPaint(browser, type, dirtyRects, buffer, width, height);
}

void DemoCefClient::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
  if (delegate_)
    delegate_->GetViewRect(browser, rect);
}

bool DemoCefClient::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) {
  if (delegate_) {
    delegate_->GetScreenInfo(browser, screen_info);
    return true;
  }
  return false;
}

void DemoCefClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  if (delegate_)
    delegate_->OnAfterCreated(browser);
}

bool DemoCefClient::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info) {
  if (delegate_)
    return delegate_->OnCursorChange(browser, cursor, type, custom_cursor_info);
  else
    return false;
}


bool DemoCefClient::DoClose(CefRefPtr<CefBrowser> browser) {
  // Allow the close. For windowed browsers this will result in the OS close event being sent.
  return false;
}

void DemoCefClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  if (delegate_) {
    delegate_->CanClose(browser);
  }
}

void DemoCefClient::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
  if (frame->IsMain()) {
    std::string url = frame->GetURL();
    std::string name = frame->GetName();
  }
}