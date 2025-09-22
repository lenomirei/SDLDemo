#include "cef/include/cef_app.h"

class DemoCefApp : public CefApp {
 public:
  DemoCefApp() = default;

 private:
  IMPLEMENT_REFCOUNTING(DemoCefApp);
};