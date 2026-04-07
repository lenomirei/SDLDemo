#include "browser_window.h"

#include <unordered_map>

class WindowsManager : BrowserWindow::Observer {
 public:
  WindowsManager() = default;
  ~WindowsManager() = default;

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

 protected:
  uint32_t next_window_id_ = 1;
  std::unordered_map<uint32_t, CefRefPtr<BrowserWindow>> windows_map_;
  std::vector<uint32_t> pending_removal_window_ids_;
};