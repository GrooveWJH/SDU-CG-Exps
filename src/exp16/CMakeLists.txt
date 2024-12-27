cmake_minimum_required(VERSION 3.16)
project(cg12_interaction VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(cg12_interaction WIN32 MACOSX_BUNDLE
    circle.cpp circle.h
    figurewidget.cpp figurewidget.h
    grid.cpp grid.h
    guide.cpp guide.h
    line.cpp line.h
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    rectex.cpp rectex.h
    ruler.cpp ruler.h
    textwidget.cpp textwidget.h
)
target_link_libraries(cg12_interaction PRIVATE
    Qt::Core
    Qt::Gui
)


# Resources:
set(Resources_resource_files
    "images/directional.png"
    "images/gravity.png"
    "images/grid.png"
    "images/guide.png"
    "images/rotate.png"
    "images/rubberband.png"
    "images/ruler.png"
    "images/scale.png"
    "images/show.png"
    "images/translate.png"
)

qt_add_resources(cg12_interaction "Resources"
    PREFIX
        "/image"
    FILES
        ${Resources_resource_files}
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(cg12_interaction PRIVATE
        Qt::Widgets
    )
endif()

install(TARGETS cg12_interaction
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET cg12_interaction
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
