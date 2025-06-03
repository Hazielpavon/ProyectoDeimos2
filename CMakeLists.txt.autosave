cmake_minimum_required(VERSION 3.16)
project(ProyectDeimos VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ——————————————
#  Encuentra Widgets y Multimedia
# ——————————————
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    pantallainicio.h
    pantallainicio.cpp
    menuopciones.h
    menuopciones.cpp
    entidad.h
    entidad.cpp
    colatareas.h
    colatareas.cpp
    hilotrabajo.h
    hilotrabajo.cpp
    tranformacion.h
    tranformacion.cpp
    sprite.h
    sprite.cpp
    enemigo.h
    enemigo.cpp
    jugador.h
    jugador.cpp
    componentefisico.h
    componentefisico.cpp
    componentesalud.h
    componentesalud.cpp
    clickablelabel.h
    clickablelabel.cpp
    pantallacarga.h
    pantallacarga.cpp
    videointro.h
    videointro.cpp
    resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ProyectDeimos
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(ProyectDeimos SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(ProyectDeimos
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# ——————————————
#  Linkea Widgets y Multimedia
# ——————————————
target_link_libraries(ProyectDeimos
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Multimedia
        Qt${QT_VERSION_MAJOR}::MultimediaWidgets
)

# ——————————————
#  Copiar carpeta Sprites/
# ——————————————
add_custom_command(
    TARGET ProyectDeimos POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Sprites
        $<TARGET_FILE_DIR:ProyectDeimos>/Sprites
)

# Opciones para iOS/macOS
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ProyectDeimos)
endif()

set_target_properties(ProyectDeimos PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ProyectDeimos
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ProyectDeimos)
endif()
