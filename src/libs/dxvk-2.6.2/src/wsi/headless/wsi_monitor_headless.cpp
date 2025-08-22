#if defined(DXVK_WSI_HEADLESS)

#include "../wsi_monitor.h"

#include "wsi/native_headless.h"
#include "wsi_platform_headless.h"

#include "../../util/util_string.h"
#include "../../util/log/log.h"

namespace dxvk::wsi {

  HMONITOR HeadlessWsiDriver::getDefaultMonitor() {
    return enumMonitors(0);
  }


  HMONITOR HeadlessWsiDriver::enumMonitors(uint32_t index) {
    return isDisplayValid(int32_t(index))
      ? toHmonitor(index)
      : nullptr;
  }

  HMONITOR HeadlessWsiDriver::enumMonitors(const LUID *adapterLUID[], uint32_t numLUIDs, uint32_t index) {
    return enumMonitors(index);
  }

  bool HeadlessWsiDriver::getDisplayName(
          HMONITOR         hMonitor,
          WCHAR            (&Name)[32]) {
    const int32_t displayId    = fromHmonitor(hMonitor);

    if (!isDisplayValid(displayId))
      return false;

    std::wstringstream nameStream;
    nameStream << LR"(\\.\DISPLAY)" << (displayId + 1);

    std::wstring name = nameStream.str();

    std::memset(Name, 0, sizeof(Name));
    name.copy(Name, name.length(), 0);

    return true;
  }


  bool HeadlessWsiDriver::getDesktopCoordinates(
          HMONITOR         hMonitor,
          RECT*            pRect) {
    const int32_t displayId    = fromHmonitor(hMonitor);

    if (!isDisplayValid(displayId))
      return false;

    pRect->left   = 0;
    pRect->top    = 0;
    pRect->right  = 1024;
    pRect->bottom = 1024;

    return true;
  }


  bool HeadlessWsiDriver::getDisplayMode(
          HMONITOR         hMonitor,
          uint32_t         ModeNumber,
          WsiMode*         pMode) {
    const int32_t displayId    = fromHmonitor(hMonitor);

    if (!isDisplayValid(displayId))
      return false;

    pMode->width        = 1024;
    pMode->height       = 1024;
    pMode->refreshRate  = WsiRational{60 * 1000, 1000};
    pMode->bitsPerPixel = 32;
    pMode->interlaced   = false;
    return true;
  }


  bool HeadlessWsiDriver::getCurrentDisplayMode(
          HMONITOR         hMonitor,
          WsiMode*         pMode) {
    const int32_t displayId    = fromHmonitor(hMonitor);

    if (!isDisplayValid(displayId))
      return false;

    pMode->width        = 1024;
    pMode->height       = 1024;
    pMode->refreshRate  = WsiRational{60 * 1000, 1000};
    pMode->bitsPerPixel = 32;
    pMode->interlaced   = false;
    return true;
  }


  bool HeadlessWsiDriver::getDesktopDisplayMode(
          HMONITOR         hMonitor,
          WsiMode*         pMode) {
    const int32_t displayId    = fromHmonitor(hMonitor);

    if (!isDisplayValid(displayId))
      return false;

    pMode->width        = 1024;
    pMode->height       = 1024;
    pMode->refreshRate  = WsiRational{60 * 1000, 1000};
    pMode->bitsPerPixel = 32;
    pMode->interlaced   = false;
    return true;
  }

  std::vector<uint8_t> HeadlessWsiDriver::getMonitorEdid(HMONITOR hMonitor) {
    Logger::err("getMonitorEdid not implemented on this platform.");
    return {};
  }

}

#endif
