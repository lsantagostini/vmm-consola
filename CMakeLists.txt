cmake_minimum_required(VERSION 3.16)

project(vmConsola VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ya no necesitamos LinguistTools, solo Widgets y Test
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Test)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        vmm_manager.cpp
        vmm_manager.h
)

# Ejecutable Principal (Limpio, sin archivos QM)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(vmConsola MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    add_executable(vmConsola ${PROJECT_SOURCES})
endif()

target_link_libraries(vmConsola PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Instalación
include(GNUInstallDirs)
install(TARGETS vmConsola
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(vmConsola)
endif()

# ==========================================
#              TESTING
# ==========================================
enable_testing()

add_executable(tst_manager
    tst_manager.cpp
    vmm_manager.cpp
    vmm_manager.h
)

target_link_libraries(tst_manager PRIVATE Qt${QT_VERSION_MAJOR}::Test)

add_test(NAME TestLogic COMMAND tst_manager)
