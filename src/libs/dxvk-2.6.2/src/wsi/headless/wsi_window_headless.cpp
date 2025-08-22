#if defined(DXVK_WSI_HEADLESS)

#include "../wsi_window.h"

#include "native/wsi/native_headless.h"
#include "wsi_platform_headless.h"

#include "../../util/util_string.h"
#include "../../util/log/log.h"

#include <windows.h>

namespace dxvk::wsi {

  void HeadlessWsiDriver::getWindowSize(
        HWND      hWindow,
        uint32_t* pWidth,
        uint32_t* pHeight) {
    if (pWidth)
      *pWidth = 1024;

    if (pHeight)
      *pHeight = 1024;
  }


  void HeadlessWsiDriver::resizeWindow(
          HWND             hWindow,
          DxvkWindowState* pState,
          uint32_t         Width,
          uint32_t         Height) {
  }


  bool HeadlessWsiDriver::setWindowMode(
          HMONITOR         hMonitor,
          HWND             hWindow,
          DxvkWindowState* pState,
    const WsiMode&         pMode) {
    const int32_t displayId    = fromHmonitor(hMonitor);

    if (!isDisplayValid(displayId))
      return false;

    return true;
  }



  bool HeadlessWsiDriver::enterFullscreenMode(
          HMONITOR         hMonitor,
          HWND             hWindow,
          DxvkWindowState* pState,
          bool             ModeSwitch) {
    const int32_t displayId    = fromHmonitor(hMonitor);

    if (!isDisplayValid(displayId))
      return false;
    return true;
  }


  bool HeadlessWsiDriver::leaveFullscreenMode(
          HWND             hWindow,
          DxvkWindowState* pState,
          bool             restoreCoordinates) {
    return true;
  }


  bool HeadlessWsiDriver::restoreDisplayMode() {
    // Don't need to do anything with SDL2 here.
    return true;
  }


  HMONITOR HeadlessWsiDriver::getWindowMonitor(HWND hWindow) {
    return toHmonitor(0);
  }


  bool HeadlessWsiDriver::isWindow(HWND hWindow) {
    return true;
  }


  bool HeadlessWsiDriver::isMinimized(HWND hWindow) {
    return false;
  }


  bool HeadlessWsiDriver::isOccluded(HWND hWindow) {
    return false;
  }


  void HeadlessWsiDriver::updateFullscreenWindow(
          HMONITOR hMonitor,
          HWND     hWindow,
          bool     forceTopmost) {
    // Don't need to do anything with SDL2 here.
  }


  VkResult HeadlessWsiDriver::createSurface(
          HWND                      hWindow,
          PFN_vkGetInstanceProcAddr pfnVkGetInstanceProcAddr,
          VkInstance                instance,
          VkSurfaceKHR*             pSurface) {
    return VK_ERROR_OUT_OF_HOST_MEMORY;
  }

}

#endif
