cmake_minimum_required(VERSION 3.0)
project(GlobalKeyLib)

set(SOURCES
  GK.h
  GKProxyApp.h
  GKProxyApp.cpp
  GKHotkey.h
  GKHotkey.cpp
  GKSystem.h
  GKSystem.cpp
  )

if (WIN32)
  set(IMP_SOURCES
    GKWin.h
    GKWin.cpp
    )
else(APPLE)
  set(IMP_SOURCES
    GKMac.h
    GKMac.mm
    )
endif()


add_library(
  GlobalKeyLib
  STATIC
  ${SOURCES}
  ${IMP_SOURCES}
  )
