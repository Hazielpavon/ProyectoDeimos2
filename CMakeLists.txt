cmake_minimum_required(VERSION 3.16)
project(ProyectDeimos2 VERSION 0.1 LANGUAGES CXX)

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

# ——————————————
#  Lista de FUENTES (CPP, H, UI y .qrc)
# ——————————————
set(PROJECT_SOURCES
    main.cpp

    # Ventanas/Clases/Cabeceras
    mainwindow.h
    mainwindow.cpp
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

    # Clase tutorial
    tutorialscene.h
    tutorialscene.cpp

    # Recursos Qt:
    resources.qrc
)

# ——————————————
#  Crea el ejecutable Qt
# ——————————————
if (QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(ProyectDeimos2
        ${PROJECT_SOURCES}
    )
    # Finaliza el bundle en Qt6 (para .app en macOS, íconos, etc.)
    qt_finalize_executable(ProyectDeimos2)
else()
    if (ANDROID)
        add_library(ProyectDeimos2 SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(ProyectDeimos2
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# ——————————————
#  Linkea Widgets y Multimedia
# ——————————————
target_link_libraries(ProyectDeimos2
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Multimedia
        Qt${QT_VERSION_MAJOR}::MultimediaWidgets
)

# ——————————————
#  Copiar carpeta Sprites/ al mismo directorio del ejecutable
# ——————————————
# … resto del CMakeLists …

# Copiar carpeta resources/Sprites al mismo directorio del ejecutable
if (EXISTS "${CMAKE_SOURCE_DIR}/resources/Sprites")
    add_custom_command(
        TARGET ProyectDeimos2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/resources/Sprites"
            "$<TARGET_FILE_DIR:ProyectDeimos2>/Sprites"
    )
endif()


# Opciones para iOS/macOS
if (${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ProyectDeimos2)
endif()

set_target_properties(ProyectDeimos2 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Instalación (si la necesitas más adelante)
include(GNUInstallDirs)
install(TARGETS ProyectDeimos2
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
