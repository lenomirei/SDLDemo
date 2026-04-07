#include "windows_manager.h"

void WindowsManager::Initialize() {
  client_ = new DemoCefClient(this);
}

void WindowsManager::CreateNewWindow(const std::string& name, bool show) {
  BrowserWindow::CreateBrowser(client_);
}

void WindowsManager::RenderAllWindows() {
  // main loop in main thread
  for (auto& pair : windows_map_) {
    CefRefPtr<BrowserWindow> window = pair.second;
    if (window) {
      window->Draw();
    }
  }

  for (uint32_t id : pending_removal_window_ids_) {
    windows_map_.erase(id);
  }
  pending_removal_window_ids_.clear();
}

void WindowsManager::CloseAllWindows() {
  for (auto& pair : windows_map_) {
    CefRefPtr<BrowserWindow> window = pair.second;
    if (window) {
      window->Close();
    }
  }
}

void WindowsManager::RemoveWindow(uint32_t id) {
  pending_removal_window_ids_.push_back(id);
}

void WindowsManager::OnBrowserWindowClosed(const uint32_t window_id) {
  RemoveWindow(window_id);
}

void WindowsManager::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) {
  auto browser_id = browser->GetIdentifier();
  if (auto it = windows_map_.find(browser_id); it != windows_map_.end()) {
    CefRefPtr<BrowserWindow> window = it->second;
    if (window) {
      window->OnPaint(browser, type, dirtyRects, buffer, width, height);
    }
  }
}

void WindowsManager::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
  auto browser_id = browser->GetIdentifier();
  if (auto it = windows_map_.find(browser_id); it != windows_map_.end()) {
    CefRefPtr<BrowserWindow> window = it->second;
    if (window) {
      window->GetViewRect(browser, rect);
    }
  }
}

bool WindowsManager::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info) {
  auto browser_id = browser->GetIdentifier();
  if (auto it = windows_map_.find(browser_id); it != windows_map_.end()) {
    CefRefPtr<BrowserWindow> window = it->second;
    if (window) {
      return window->OnCursorChange(browser, cursor, type, custom_cursor_info);
    }
  }
  return false;
}

void WindowsManager::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) {
  auto browser_id = browser->GetIdentifier();
  if (auto it = windows_map_.find(browser_id); it != windows_map_.end()) {
    CefRefPtr<BrowserWindow> window = it->second;
    if (window) {
      window->GetScreenInfo(browser, screen_info);
    }
  }
}

void WindowsManager::CanClose(CefRefPtr<CefBrowser> browser) {
  auto browser_id = browser->GetIdentifier();
  if (auto it = windows_map_.find(browser_id); it != windows_map_.end()) {
    CefRefPtr<BrowserWindow> window = it->second;
    if (window) {
      window->CanClose();
    }
  }
}

void WindowsManager::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  if (browser->IsPopup())
    return;
  auto browser_id = browser->GetIdentifier();
  if (windows_map_.find(browser_id) == windows_map_.end()) {
    CefRefPtr<BrowserWindow> new_window = new BrowserWindow("new window" + std::to_string(browser_id), browser_id, browser, this, true);
    windows_map_[browser_id] = new_window;
  } else {
    // TODO(lenomirei): handle the case when the browser already exists in the map, maybe update the existing window's browser reference
  }
}