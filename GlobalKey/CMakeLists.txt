cmake_minimum_required(VERSION 3.0)
set(PROJECT_NAME GlobalKey)
project(${PROJECT_NAME})

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

if (WIN32)
  set(SOURCES
    GK.h
    GK.cpp
    GKWin.h
    GKWin.cpp
    GKProxyApp.h
    GKProxyApp.cpp
    GKHotKey.h
    GKHotKey.cpp
    GKConfig.h
    GKConfig.cpp
    GKTask.h
    GKTask.cpp
    win32/App.cpp
    win32/Resource.h
    win32/Resource.rc
  )

  add_executable(
    ${PROJECT_NAME}
    WIN32
    ${SOURCES}
  )
else()

  set(CMAKE_AUTOMOC ON)
  set(CMAKE AUTOUIC ON)
  set(CMAKE_AUTORCC ON)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  find_package(Qt5Core REQUIRED)
  find_package(Qt5Gui REQUIRED)
  find_package(Qt5Widgets REQUIRED)
  find_package(Qt5Network REQUIRED)

  if (WIN32)
    find_package(Qt5WinExtras CONFIG REQUIRED)
  endif()

  set(UI
    #GKAboutDialog.ui
    #GKDialog.ui
    #GKWidget.ui
    #GKWidgetItem.ui
    )

  set(SOURCES
    GK.h
    GK.cpp
    GKMac.h
    GKMac.mm
    Resources/Resources.qrc
    main.cpp
    )

  qt5_wrap_ui(UI_HEADERS ${UI})

  if (APPLE)
    set(
      RESOURCE_FILES
      macOS/Assets.xcassets
      )
    add_executable(
      ${PROJECT_NAME}
      MACOSX_BUNDLE
      ${SOURCES}
      ${UI}
      ${UI_HEADERS}
      ${RESOURCE_FILES}
      )
  elseif(WIN32)
    set(
      RESOURCE_FILES
      win32/Resource.h
      win32/Resource.rc
      )
    add_executable(
      ${PROJECT_NAME}
    WIN32
      ${SOURCES}
      ${UI}
      ${UI_HEADERS}
      
      ${RESOURCE_FILES}
      )

    # Disable console in Release
    #if(CMAKE_BUILD_TYPE STREQUAL "Release")
    #  set_property(TARGET main PROPERTY WIN32_EXECUTABLE true)
    #endif()

  else()
    add_executable(
      ${PROJECT_NAME}
      ${SOURCES}
      ${UI}
      ${UI_HEADERS}
      )
  endif()


  target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core)
  target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Gui)
  target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)
  target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Network)
  if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::WinExtras Qt5::WinExtrasPrivate)
  elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa -framework AppKit")
  endif()

  target_link_libraries(${PROJECT_NAME}  PRIVATE QHotkey::QHotkey)
  target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/QHotKey/QHotKey
    )

endif()


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

    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.11"
    
    RESOURCE "${RESOURCE_FILES}"    
    )

#  ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
#    POST_BUILD
#    COMMAND /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/CopyLibs.sh "${CMAKE_CURRENT_BINARY_DIR}/\${CONFIGURATION}/${APP_NAME}.app/Contents/MacOS/${APP_NAME}"
#  )
endif()
