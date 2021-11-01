QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BTree/btree.cpp \
    BTree/btreenode.cpp \
    Delete/deletewindow.cpp \
    ImportCSV/columnnameswindow.cpp \
    Insert/insertwindow.cpp \
    main.cpp \
    Main/mainwindow.cpp \
    NewDB/newdbwindow.cpp \
    NewTable/newtablewindow.cpp \
    Pager/pager.cpp \
    Ptr/ptr.cpp \
    Select/selectwindow.cpp \
    Table/table.cpp \
    Update/updatewindow.cpp \
    Value/value.cpp

HEADERS += \
    ImportCSV/columnnameswindow.h \
    Value/base.h \
    BTree/btree.h \
    BTree/btreenode.h \
    db_constants.h \
    Delete/deletewindow.h \
    Value/float.h \
    Insert/insertwindow.h \
    Value/int.h \
    Main/mainwindow.h \
    NewDB/newdbwindow.h \
    NewTable/newtablewindow.h \
    Pager/pager.h \
    Ptr/ptr.h \
    Select/selectwindow.h \
    Value/string_val.h \
    Table/table.h \
    Update/updatewindow.h \
    Value/value.h

FORMS += \
    Delete/deletewindow.ui \
    ImportCSV/columnnameswindow.ui \
    Insert/insertwindow.ui \
    Main/mainwindow.ui \
    NewDB/newdbwindow.ui \
    NewTable/newtablewindow.ui \
    Select/selectwindow.ui \
    Update/updatewindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
