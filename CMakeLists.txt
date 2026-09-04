cmake_minimum_required(VERSION 3.16)

project(Sorta VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt 6
find_package(Qt6 6.2 COMPONENTS Core Gui Widgets REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
)

# Core sources
set(CORE_SOURCES
    src/core/FileItem.h
    src/core/FileScanner.h
    src/core/FileScanner.cpp
    src/core/RenameEngine.h
    src/core/RenameEngine.cpp
    src/core/ConflictDetector.h
    src/core/ConflictDetector.cpp
    src/core/OrganizeEngine.h
    src/core/OrganizeEngine.cpp
    src/core/OperationHistory.h
    src/core/OperationHistory.cpp
)

# UI sources
set(UI_SOURCES
    src/ui/FileTableModel.h
    src/ui/FileTableModel.cpp
    src/ui/RenamePanel.h
    src/ui/RenamePanel.cpp
    src/ui/OrganizePanel.h
    src/ui/OrganizePanel.cpp
    src/ui/HistoryDialog.h
    src/ui/HistoryDialog.cpp
    src/ui/AboutDialog.h
    src/ui/AboutDialog.cpp
    src/ui/MainWindow.h
    src/ui/MainWindow.cpp
)

# Resources
set(RESOURCES
    resources/resources.qrc
)

# Windows specific resources (Icon and metadata)
if(WIN32)
    set(WINDOWS_RESOURCES
        resources/app.rc
    )
endif()

add_executable(Sorta WIN32
    src/main.cpp
    ${CORE_SOURCES}
    ${UI_SOURCES}
    ${RESOURCES}
    ${WINDOWS_RESOURCES}
)

target_link_libraries(Sorta PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

if(MSVC)
    target_compile_options(Sorta PRIVATE /W4 /utf-8)
else()
    target_compile_options(Sorta PRIVATE -Wall -Wextra)
endif()

# Installation rules for deployment
install(TARGETS Sorta
    BUNDLE DESTINATION .
    RUNTIME DESTINATION .
)
