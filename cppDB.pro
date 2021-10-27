QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    btree.cpp \
    btreenode.cpp \
    deletewindow.cpp \
    insertwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    newdbwindow.cpp \
    newtablewindow.cpp \
    pager.cpp \
    ptr.cpp \
    selectwindow.cpp \
    table.cpp \
    updatewindow.cpp \
    value.cpp

HEADERS += \
    base.h \
    btree.h \
    btreenode.h \
    db_constants.h \
    deletewindow.h \
    float.h \
    insertwindow.h \
    int.h \
    mainwindow.h \
    newdbwindow.h \
    newtablewindow.h \
    pager.h \
    ptr.h \
    selectwindow.h \
    string_val.h \
    table.h \
    updatewindow.h \
    value.h

FORMS += \
    deletewindow.ui \
    insertwindow.ui \
    mainwindow.ui \
    newdbwindow.ui \
    newtablewindow.ui \
    selectwindow.ui \
    updatewindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
