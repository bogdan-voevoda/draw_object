QT       += core gui widgets opengl

TARGET    = template
TEMPLATE  = app

SOURCES += main.cpp\
    mainwindow.cpp \
    mainwidget.cpp \
    camera.cpp \
    render_object.cpp \
    obj_loader/objloader.cpp \
    skybox/skybox.cpp \
    terrain/terrain.cpp \
    draw_object.cpp

HEADERS  += mainwindow.h \
    mainwidget.h \
    camera.h \
    render_object.h \
    obj_loader/objloader.h \
    skybox/skybox.h \
    terrain/terrain.h \
    draw_object.h

FORMS    += mainwindow.ui

CONFIG   += mobility
CONFIG   += c++11

RESOURCES += \
            textures/textures.qrc \
            shaders/shaders.qrc \
            3d_models/3d_models.qrc
