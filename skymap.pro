#-------------------------------------------------
#
# Project created by QtCreator 2019-04-06T18:09:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets \
                                        concurrent

TARGET = skymap
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES = \
    src\main.cpp \
    src\mainwindow.cpp \
    src\CSVAdapter.cpp \
    src\extraction.cpp \
    src\ImageProcessing.cpp \
    src\MyFunctions.cpp \
    src\NoOptic.cpp \
    src\Sky.cpp \
    src\SkyMap.cpp \
    src\SkyMapMatching.cpp \
    src\Star.cpp \
    src\TriangleMatching.cpp \
    src/QCSVAdapter.cpp \
    src/exif.cpp \
    src/RCFI.cpp

HEADERS += \
    include\mainwindow.h \
    include\CSVAdapter.h \
    include\extraction.h \
    include\ImageProcessing.h \
    include\MyFunctions.h \
    include\NoOptic.h \
    include\Sky.h \
    include\SkyMap.h \
    include\SkyMapMatching.h \
    include\Star.h \
    include\TriangleMatching.h \
    include/QCSVAdapter.h \
    include/exif.h \
    #include/ConsolColor.h
    include/RCFI.h \
    include/commonhelper.h

FORMS += \
    form\mainwindow.ui

INCLUDEPATH = \
    include \
    include\opencv2 \

LIBS += \
    $$PWD\lib\libopencv_core401.dll.a \
    $$PWD\lib\libopencv_highgui401.dll.a \
    $$PWD\lib\libopencv_imgproc401.dll.a \
    $$PWD\lib\libopencv_imgcodecs401.dll.a \
    $$PWD\lib\libopencv_features2d401.dll.a \
    $$PWD\lib\libopencv_objdetect401.dll.a \


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

