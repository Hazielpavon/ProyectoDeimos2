cmake_minimum_required(VERSION 3.16)
project(ProyectDeimos2 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# macOS config
set(CMAKE_OSX_ARCHITECTURES arm64)
set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk")
set(CMAKE_BUILD_PARALLEL_LEVEL 1)

# ——————————————
#  Encuentra Widgets y Multimedia
# ——————————————
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)

# ——————————————
#  Fuentes del proyecto
# ——————————————
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    pantallainicio.cpp pantallainicio.h
    menuopciones.cpp menuopciones.h
    entidad.cpp entidad.h
    ObjetosYColisiones.cpp ObjetosYColisiones.h
    BringerOfDeath.cpp BringerOfDeath.h
    CombateManager.cpp CombateManager.h
    tranformacion.cpp tranformacion.h
    ciudadinversa.cpp ciudadinversa.h
    sprite.cpp sprite.h
    enemigo.cpp enemigo.h
    jugador.cpp jugador.h
    componentefisico.cpp componentefisico.h
    componentesalud.cpp componentesalud.h
    clickablelabel.cpp clickablelabel.h
    pantallacarga.cpp pantallacarga.h
    videointro.cpp videointro.h
    tutorialscene.cpp tutorialscene.h
    nivelraicesolvidadas.cpp nivelraicesolvidadas.h
    mapawidget.cpp mapawidget.h
    grafomapa.cpp grafomapa.h
    niveltorredelamarca.cpp niveltorredelamarca.h
    demon.cpp demon.h
    resources.qrc
)

# ——————————————
#  Crear ejecutable
# ——————————————
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ProyectDeimos2
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources/Torre_De_La_Marca.png
        resources/Torre_De_La_Marca2.png
    )
else()
    add_executable(ProyectDeimos2 ${PROJECT_SOURCES})
endif()

# ——————————————
#  Linkea bibliotecas Qt
# ——————————————
target_link_libraries(ProyectDeimos2
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Multimedia
        Qt${QT_VERSION_MAJOR}::MultimediaWidgets
)

# ——————————————
#  Copiar carpeta Sprites al output
# ——————————————
add_custom_command(
    TARGET ProyectDeimos2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Sprites
        $<TARGET_FILE_DIR:ProyectDeimos2>/Sprites
)

# ——————————————
#  Propiedades para macOS
# ——————————————
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ProyectDeimos2)
endif()

set_target_properties(ProyectDeimos2 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# ——————————————
#  Instalación
# ——————————————
include(GNUInstallDirs)
install(TARGETS ProyectDeimos2
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ——————————————
#  Finalizar para Qt 6
# ——————————————
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ProyectDeimos2)
endif()
