cmake_minimum_required(VERSION 3.0)
set(PROJECT_NAME GlobalKey)
project(${PROJECT_NAME})

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

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
  GKApp.h
  GKApp.cpp
  GKAppFactory.h
  GKAppFactory.cpp
  GKConfig.h
  GKConfig.cpp
  GKMacImp.h
  GKMacImp.mm
  main.cpp
  )

qt5_wrap_ui(UI_HEADERS ${UI})
add_executable(
  ${PROJECT_NAME}
  ${SOURCES}
  ${UI}
  ${UI_HEADERS}
  )

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

