# Ensure the project is built only on macOS
!macx: error("This project can only be built on macOS")

QT += core gui opengl widgets openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# Uncomment the following line to disable deprecated APIs
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    main.cpp \
    widget.cpp

HEADERS += \
    widget.h

# Deployment rules and library linking for macOS
macx {
    target.path = /Applications/$${TARGET}.app/Contents/MacOS
    INSTALLS += target

    # Include paths for FreeGLUT
    INCLUDEPATH += /opt/homebrew/include

    # Library paths and linking
    LIBS += -L/opt/homebrew/lib -lfreeglut -framework OpenGL
}

# Resource files
RESOURCES += \
    textures.qrc