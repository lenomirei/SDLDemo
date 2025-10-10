#include "windows_manager.h"

void WindowsManager::CreateNewWindow(const std::string& name, bool show) {
  if (windows_map_.find(next_window_id_) == windows_map_.end()) {
    CefRefPtr<BrowserWindow> new_window = new BrowserWindow(name, show);
    windows_map_[next_window_id_] = new_window;
    next_window_id_++;
  }
}

void WindowsManager::RenderAllWindows() {
  for (auto& pair : windows_map_) {
    CefRefPtr<BrowserWindow> window = pair.second;
    if (window) {
      window->Draw();
    }
  }
}

void WindowsManager::CloseAllWindows() {
  for (auto& pair : windows_map_) {
    CefRefPtr<BrowserWindow> window = pair.second;
    if (window) {
      window->Close();
    }
  }
}

