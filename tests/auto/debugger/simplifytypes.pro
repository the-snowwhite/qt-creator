QTC_LIB_DEPENDS += utils
include(../qttest.pri)
QT += network
QT -= gui widgets

DEBUGGERDIR = $$IDE_SOURCE_TREE/src/plugins/debugger

INCLUDEPATH += $$DEBUGGERDIR

SOURCES += \
    tst_simplifytypes.cpp \
    $$DEBUGGERDIR/debuggerprotocol.cpp \