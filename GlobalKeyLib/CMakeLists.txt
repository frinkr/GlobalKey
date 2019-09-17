cmake_minimum_required(VERSION 3.0)
project(GlobalKeyLib)

set(SOURCES
  GK.h
  GKProxyApp.h
  GKProxyApp.cpp
  GKHotkey.h
  GKHotkey.cpp
  )

if (WIN32)
  set(IMP_SOURCES
    GKHotkeyWin.h
    GKHotkeyWin.cpp
    GKProxyAppWin.h
    GKProxyAppWin.cpp
    )
else(APPLE)
  set(IMP_SOURCES
    GKHotkeyMac.h
    GKHotkeyMac.mm
    GKProxyAppMac.h
    GKProxyAppMac.mm
    )
endif()


add_library(
  GlobalKeyLib
  STATIC
  ${SOURCES}
  ${IMP_SOURCES}
  )
