/*
 * @Author:
 * @Date: 2025-09-19 18:45:38
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2025-09-19 19:32:32
 * @FilePath: \SDLDemo\src\demo_cef_client.cc
 * @Description:
 *
 */
#include "demo_cef_client.h"

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
  if (!browser->IsPopup())
    browser_ = browser;
}

CefRefPtr<CefBrowser> DemoCefClient::GetBrowser() {
  return browser_;
}

bool DemoCefClient::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info) {
  if (delegate_)
    return delegate_->OnCursorChange(browser, cursor, type, custom_cursor_info);
  else
    return false;
}

void DemoCefClient::CloseBrowser() {
  browser_->GetHost()->CloseBrowser(false);
}

bool DemoCefClient::DoClose(CefRefPtr<CefBrowser> browser) {
  // Allow the close. For windowed browsers this will result in the OS close event being sent.
  return false;
}

void DemoCefClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  if (browser_ && browser_->IsSame(browser)) {
    browser_ = nullptr;
  }

  if (delegate_) {
    delegate_->CanClose();
    delegate_ = nullptr;
  }
}