#if defined(DXVK_WSI_HEADLESS)

#include "wsi_platform_headless.h"
#include "../../util/util_error.h"
#include "../../util/util_string.h"
#include "../../util/util_win32_compat.h"

namespace dxvk::wsi {

  HeadlessWsiDriver::HeadlessWsiDriver() {
  }

  HeadlessWsiDriver::~HeadlessWsiDriver() {
  }

  std::vector<const char *> HeadlessWsiDriver::getInstanceExtensions() {
    return std::vector<const char *>(0);
  }

  static bool createHeadlessWsiDriver(WsiDriver **driver) {
    try {
      *driver = new HeadlessWsiDriver();
    } catch (const DxvkError& e) {
      return false;
    }
    return true;
  }

  WsiBootstrap HeadlessWSI = {
    "Headless",
    createHeadlessWsiDriver
  };

}

#endif
