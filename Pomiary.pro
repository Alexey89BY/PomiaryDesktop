QT       += core gui multimedia multimediawidgets serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialoghistory.cpp \
    dialogsettings.cpp \
    formpoint.cpp \
    formrange.cpp \
    globals.cpp \
    main.cpp \
    mainwindow.cpp \
    modeselectionwindow.cpp \
    pointdata.cpp \
    pointrange.cpp \
    settings.cpp \
    qgridcameraviewfinder.cpp \
    windowanalyse.cpp

HEADERS += \
    dialoghistory.h \
    dialogsettings.h \
    formpoint.h \
    formrange.h \
    globals.h \
    mainwindow.h \
    modeselectionwindow.h \
    pointdata.h \
    pointrange.h \
    settings.h \
    qgridcameraviewfinder.h \
    windowanalyse.h

FORMS += \
    dialoghistory.ui \
    dialogsettings.ui \
    formpoint.ui \
    formrange.ui \
    mainwindow.ui \
    modeselectionwindow.ui \
    windowanalyse.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
