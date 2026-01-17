cmake_minimum_required(VERSION 3.16)

project(vmConsola VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Agregamos componente "Test"
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Test)

# Archivos fuente principales
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        vmm_manager.cpp
        vmm_manager.h
)

set(TS_FILES vmConsola_en.ts)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

# Ejecutable Principal
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(vmConsola MANUAL_FINALIZATION ${PROJECT_SOURCES} ${QM_FILES})
else()
    add_executable(vmConsola ${PROJECT_SOURCES} ${QM_FILES})
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
#              ZONA DE TESTING
# ==========================================
enable_testing()

# Creamos el ejecutable de pruebas
add_executable(tst_manager
    tst_manager.cpp
    vmm_manager.cpp
    vmm_manager.h
)

# Vinculamos con la librería de Test de Qt
target_link_libraries(tst_manager PRIVATE Qt${QT_VERSION_MAJOR}::Test)

# Registramos el test para que CTest lo reconozca
add_test(NAME TestLogica COMMAND tst_manager)
