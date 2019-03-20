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
  GKAboutDialog.ui
  GKDialog.ui
  GKWidget.ui
  GKWidgetItem.ui
  )

set(SOURCES
  GKAboutDialog.cpp
  GKAboutDialog.h
  GKApplication.cpp
  GKApplication.h
  GKDialog.cpp
  GKDialog.h
  GKItem.cpp
  GKItem.h
  GKWidget.cpp
  GKWidget.h
  GKWidgetItem.cpp
  GKWidgetItem.h
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
endif()
