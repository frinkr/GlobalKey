cmake_minimum_required(VERSION 3.0)
set(PROJECT_NAME GlobalKey)
project(${PROJECT_NAME})

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

set(SOURCES
  GKArgs.h
  GKArgs.cpp
  GKCoreApp.h
  GKCoreApp.cpp
  GKConfig.h
  GKConfig.cpp
  GKCommand.h
  GKCommand.cpp
  GKSystemService.h
  )

if (WIN32)
  add_executable(
    ${PROJECT_NAME}
    WIN32
    ${SOURCES}
    GKSystemServiceWin.cpp
    win32/main.cpp
    win32/Resource.h
    win32/Resource.rc
    )
else(APPLE)
  add_executable(
    ${PROJECT_NAME}
    MACOSX_BUNDLE
    ${SOURCES}
    GKSystemServiceMac.mm
    macOS/main.mm
    macOS/AppDelegate.h
    macOS/AppDelegate.mm
    macOS/ToastMessageWindow.h
    macOS/ToastMessageWindow.m
    macOS/Assets.xcassets
    )
  target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa -framework AppKit -framework Carbon -framework AudioToolbox -framework CoreAudio -framework QuartzCore -framework IOKit")
endif()

find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp)
  
target_link_libraries(${PROJECT_NAME} PRIVATE GlobalKeyLib)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/GlobalKeyLib)


########################### Mac Bundle #####################

if (APPLE)
  
  set(APP_VERSION "1.0.0" CACHE STRING "The version of the application")
  set(APP_NAME ${PROJECT_NAME})

  set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    MACOSX_BUNDLE YES

    # bundle config
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/macOS/plist.in
    MACOSX_BUNDLE_ICON_FILE "AppIcon"
    MACOSX_BUNDLE_BUNDLE_VERSION ${APP_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${APP_VERSION}
    MACOSX_BUNDLE_LONG_VERSION_STRING ${APP_VERSION}
    MACOSX_BUNDLE_COPYRIGHT "Copyright © 2018 Yuqing Jiang. All rights reserved."
    
    XCODE_ATTRIBUTE_PRODUCT_NAME   ${APP_NAME}
    XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER   "me.frinkr.globalkey"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
    XCODE_ATTRIBUTE_INSTALL_PATH "${CMAKE_BINARY_DIR}/ProductRelease"
    XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon"
    XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "NO"

    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.15"
    
    RESOURCE "macOS/Assets.xcassets"    
    )

  #  ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
  #    POST_BUILD
  #    COMMAND /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/CopyLibs.sh "${CMAKE_CURRENT_BINARY_DIR}/\${CONFIGURATION}/${APP_NAME}.app/Contents/MacOS/${APP_NAME}"
  #  )
endif()
