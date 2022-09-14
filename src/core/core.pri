QT += core gui widgets opengl openglwidgets

INCLUDEPATH += \
    $$PWD

LIBS += opengl32.lib

SOURCES += \
   $$PWD/axes.cc \
   $$PWD/camera.cc \
   $$PWD/datalayer.cc \
   $$PWD/fbselect.cc \
   $$PWD/helpdialog.cc \
   $$PWD/materialhandler.cc \
   $$PWD/mesh.cc \
   $$PWD/shaderhandler.cc \
   $$PWD/shaderwatcher.cc

HEADERS += \
   $$PWD/axes.h \
   $$PWD/camera.h \
   $$PWD/datalayer.h \
   $$PWD/fbselect.h \
   $$PWD/global.h \
   $$PWD/helpdialog.h \
   $$PWD/materialhandler.h \
   $$PWD/mesh.h \
   $$PWD/shaderhandler.h \
   $$PWD/shaderwatcher.h
