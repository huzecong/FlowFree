#-------------------------------------------------
#
# Project created by QtCreator 2015-08-25T11:08:01
#
#-------------------------------------------------

QT		+= core gui
QT		+= qml quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#QMAKE_CXXFLAGS += -std=c++11

TARGET = FlowFree
TEMPLATE = app

VERSION = 1.0

SOURCES += main.cpp \
	gamelogic.cpp \
    gamesolver.cpp

HEADERS  += \
	gamelogic.h \
    gamesolver.h

FORMS    +=

DISTFILES += \
	main.qml \
	GameBoard.qml \
	GameBoardToggle.js \
	BoardGrid.qml \
	BoardCircle.qml \
	BoardLine.qml \
	levels/easy-1.txt \
	levels/medium-1.txt \
	ClickableSquare.qml \
	levels/easy-2.txt \
	levels/easy-3.txt \
	levels/easy-4.txt \
	levels/easy-5.txt \
	levels/expert-1.txt \
	levels/hard-1.txt \
	README.md \
	levels/expert-2.txt \
	levels/expert-3.txt \
	levels/expert-4.txt \
	levels/expert-5.txt \
	levels/hard-2.txt \
	levels/hard-3.txt \
	levels/hard-4.txt \
	levels/hard-5.txt \
	levels/medium-2.txt \
	levels/medium-3.txt \
	levels/medium-4.txt \
	levels/medium-5.txt \
	sound/Pop.wav \
	sound/Submarine.wav \
	sound/Hero.wav \
	sound/Blow.wav \
	FollowMouseCircle.qml \
	icon/icon.ico \
    HelpDialog.qml \
    MainView.qml \

RESOURCES += \
	qml.qrc

macx {
	ICON = icon/icon.icns
}

win32 {
        RC_ICONS = icon/icon.ico
}

