cmake_minimum_required(VERSION 3.16)
project(ProyectDeimos2 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES arm64)
set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk")
set(CMAKE_BUILD_PARALLEL_LEVEL 1)

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
  drop.h
  drop.cpp
  Fireball.cpp
  InventoryWidget.cpp          # <— añade esto
  npc_tutorial.cpp             # <— y esto (o como se llame realmente)
  tutorialscene.cpp            # ya lo tenías
  nivelraicesolvidadas.cpp
  Fireball.h
  pantallainicio.cpp
  menuopciones.h
  menuopciones.cpp
  entidad.h
  entidad.cpp
  ObjetosYColisiones.cpp
  BringerOfDeath.cpp
  CombateManager.cpp
  BringerOfDeath.h
  CombateManager.h
  ObjetosYColisiones.h
  tranformacion.h
  tranformacion.cpp
  ciudadinversa.h
  ciudadinversa.cpp
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
  SpriteSheetLoader.h
  Skeleton.h
  Skeleton.cpp

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(ProyectDeimos2
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    tutorialscene.h tutorialscene.cpp
    nivelraicesolvidadas.h nivelraicesolvidadas.cpp
    mapawidget.h mapawidget.cpp
    grafomapa.h grafomapa.cpp
    niveltorredelamarca.h niveltorredelamarca.cpp
    resources/Torre_De_La_Marca.png resources/Torre_De_La_Marca2.png
    ciudadinversa.h ciudadinversa.cpp
    demon.h demon.cpp
    SpriteSheetLoader.h
    skeleton.h skeleton.cpp
    minotaur.h minotaur.cpp
    monsterfly.h monsterfly.cpp
    mutantworm.h mutantworm.cpp
    carnivore.h carnivore.cpp
    cannon.h cannon.cpp
    bullet.h bullet.cpp
    npc.h npc.cpp
    mentevacia.h mentevacia.cpp
<<<<<<< HEAD
    maquina_olvido.h maquina_olvido.cpp
    plataforma_movil_shared.h
    npc_final.h npc_final.cpp
    introvideoplayer.h introvideoplayer.cpp
=======
    skilltreewidget.h skilltreewidget.cpp

>>>>>>> origin/fix2


  )
else()
  if(ANDROID)
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
#  Copiar carpeta Sprites/
# ——————————————
add_custom_command(
  TARGET ProyectDeimos2 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/Sprites
  $<TARGET_FILE_DIR:ProyectDeimos2>/Sprites
)

# Opciones para iOS/macOS
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

include(GNUInstallDirs)
install(TARGETS ProyectDeimos2
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(ProyectDeimos2)
endif()
