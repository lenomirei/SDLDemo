#include "browser_window.h"

#include <unordered_map>

class WindowsManager {
 public:
  WindowsManager() = default;
  ~WindowsManager() = default;

  void CreateNewWindow(const std::string& name = "new window", bool show = true);

  void RemoveWindow(uint32_t id) {
    windows_map_.erase(id);
  }

  CefRefPtr<BrowserWindow> GetWindow(uint32_t id) {
    auto it = windows_map_.find(id);
    if (it != windows_map_.end()) {
      return it->second;
    }
    return nullptr;
  }

  void RenderAllWindows();

  void CloseAllWindows();
  
 protected:
  uint32_t next_window_id_ = 1;
  std::unordered_map<uint32_t, CefRefPtr<BrowserWindow>> windows_map_;
};