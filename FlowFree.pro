#-------------------------------------------------
#
# Project created by QtCreator 2015-08-25T11:08:01
#
#-------------------------------------------------

QT       += core gui
QT       += qml quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#QMAKE_CXXFLAGS += -std=c++11

TARGET = FlowFree
TEMPLATE = app


SOURCES += main.cpp \
    gamelogic.cpp

HEADERS  += \
    gamelogic.h

FORMS    +=

DISTFILES += \
	FlowFreeMainView.qml \
    main.qml \
    GameBoard.qml \
    GameBoardToggle.js \
    BoardGrid.qml \
    BoardCircle.qml \
    BoardLine.qml \
    levels/easy-1.txt \
    levels/medium-1.txt

RESOURCES += \
    qml.qrc
