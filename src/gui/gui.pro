QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += core gui widgets opengl openglwidgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include($$PWD/../core/core.pri)

SOURCES += \
    glview.cc \
    main.cc \
    mainwindow.cc \
    readme.cc \
    sessionmanager.cc

HEADERS += \
    glview.h \
    mainwindow.h \
    readme.h \
    sessionmanager.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
   resources.qrc

DISTFILES += \
   sessionfile.qmodel
