#include "windows_manager.h"

void WindowsManager::CreateNewWindow(const std::string& name, bool show) {
  if (windows_map_.find(next_window_id_) == windows_map_.end()) {
    
    CefRefPtr<BrowserWindow> new_window = new BrowserWindow(name, next_window_id_, this, show);
    windows_map_[next_window_id_] = new_window;
    next_window_id_++;
  }
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